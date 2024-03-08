
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "vk_context.h"

const char* window_title = "vk-cube";
const uint32_t window_width = 1024;
const uint32_t window_height = 768;

SDL_Window* g_window = NULL;

VkRenderPass     g_render_pass = NULL;
VkImageView      g_image_views[VK_CTX_NUM_SWAPCHAIN_BUFFERS];
VkFramebuffer    g_framebuffer = NULL;
VkShaderModule   g_vertex_shader_module = NULL;
VkShaderModule   g_fragment_shader_module = NULL;
VkPipelineLayout g_pipeline_layout = NULL;
VkPipeline       g_graphics_pipeline = NULL;

bool initialize(void)
{
    bool status = true;

    uint32_t ext_count = 0;
    const char** ext_array = NULL;

    VkSurfaceKHR surface = NULL;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Could not initialize SDL\n");
        status = false;
    }

    if(status)
    {
        if(SDL_Vulkan_LoadLibrary(NULL) != 0)
        {
            printf("Could not load the vulkan library\n");
            status = false;
        }
    }

    if(status)
    {
        g_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

        if (g_window == NULL)
        {
            status = false;
            printf("Could not create SDL window\n");
        }
    }

    if(status)
    {
        if(SDL_Vulkan_GetInstanceExtensions(g_window, &ext_count, NULL) != SDL_TRUE)
        {
            status = false;
            printf("Could not get required SDL extension count\n");
        }
    }

    if(status)
    {
        ext_array = malloc(ext_count * sizeof(char*));

        if(ext_array == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(SDL_Vulkan_GetInstanceExtensions(g_window, &ext_count, ext_array) != SDL_TRUE)
        {
            status = false;
            printf("Could not get required SDL extension count\n");
        }
    }

    if(status && (ext_count > 0))
    {
        printf("Required SDL extensions:\n");
        for(uint32_t i = 0; i < ext_count; i++)
        {
            printf("\t%s\n", ext_array[i]);
        }
    }

    if(status)
    {
        status = initialize_vulkan_context(SDL_Vulkan_GetVkGetInstanceProcAddr(), ext_count, ext_array);
    }

    if(status)
    {
        if(SDL_Vulkan_CreateSurface(g_window, vk_ctx->instance, (VkSurfaceKHR*) &surface) != SDL_TRUE)
        {
            printf("Could not create vulkan surface\n");
            status = false;
        }
    }

    if(status)
    {
        status = initialize_swapchain(surface);
    }

    if(status)
    {
        VkAttachmentDescription attachment_description;
        attachment_description.flags = 0;
        attachment_description.format = vk_ctx->surface_format;
        attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_description.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference attachment_reference;
        attachment_reference.attachment = 0;
        attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass_description;
        subpass_description.flags = 0;
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.inputAttachmentCount = 0;
        subpass_description.pInputAttachments = NULL;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &attachment_reference;
        subpass_description.pResolveAttachments = NULL;
        subpass_description.pDepthStencilAttachment = NULL;
        subpass_description.preserveAttachmentCount = 0;
        subpass_description.pPreserveAttachments = NULL;

        VkRenderPassCreateInfo render_pass_info;
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.pNext = NULL;
        render_pass_info.flags = 0;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &attachment_description;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass_description;
        render_pass_info.dependencyCount = 0;
        render_pass_info.pDependencies = NULL;

        if(vk_ctx->create_render_pass(vk_ctx->device, &render_pass_info, vk_ctx->allocation_callbacks, &g_render_pass) != VK_SUCCESS)
        {
            printf("Failed to create render pass\n");
            status = false;
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < VK_CTX_NUM_SWAPCHAIN_BUFFERS; i++)
        {
            VkImageViewCreateInfo params;
            params.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            params.pNext = NULL;
            params.flags = 0;
            params.image = vk_ctx->swapchain_images[i];
            params.viewType = VK_IMAGE_VIEW_TYPE_2D;
            params.format = vk_ctx->surface_format;
            params.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            params.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            params.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            params.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            params.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            params.subresourceRange.baseMipLevel = 0;
            params.subresourceRange.levelCount = 1;
            params.subresourceRange.baseArrayLayer = 0;
            params.subresourceRange.layerCount = 1;

            if(vk_ctx->create_image_view(vk_ctx->device, &params, vk_ctx->allocation_callbacks, &g_image_views[i]) != VK_SUCCESS)
            {
                printf("Failed to create swapchain image view\n");
                status = false;
            }
        }
    }

    if(status)
    {
        VkFramebufferCreateInfo params;
        params.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        params.pNext = NULL;
        params.flags = 0;
        params.renderPass = g_render_pass;
        params.attachmentCount = 1;
        params.pAttachments = g_image_views;
        params.width = window_width;
        params.height = window_height;
        params.layers = 1;

        if(vk_ctx->create_framebuffer(vk_ctx->device, &params, vk_ctx->allocation_callbacks, &g_framebuffer) != VK_SUCCESS)
        {
            printf("Failed to create framebuffer\n");
            status = false;
        }
    }

    if(status)
    {
        VkShaderModuleCreateInfo vertex_shader_info;
        vertex_shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertex_shader_info.pNext = NULL;
        vertex_shader_info.flags = 0;
        vertex_shader_info.codeSize = 0;
        vertex_shader_info.pCode = NULL;

        if(vk_ctx->create_shader_module(vk_ctx->device, &vertex_shader_info, vk_ctx->allocation_callbacks, &g_vertex_shader_module) != VK_SUCCESS)
        {
            printf("Failed to create vertex shader\n");
            status = false;
        }
    }

    if(status)
    {
        VkShaderModuleCreateInfo fragment_shader_info;
        fragment_shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragment_shader_info.pNext = NULL;
        fragment_shader_info.flags = 0;
        fragment_shader_info.codeSize = 0;
        fragment_shader_info.pCode = NULL;

        if(vk_ctx->create_shader_module(vk_ctx->device, &fragment_shader_info, vk_ctx->allocation_callbacks, &g_vertex_shader_module) != VK_SUCCESS)
        {
            printf("Failed to create fragment shader\n");
            status = false;
        }
    }

    if(status)
    {
        VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info[2];

        pipeline_shader_stage_create_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipeline_shader_stage_create_info[0].pNext = NULL;
        pipeline_shader_stage_create_info[0].flags = 0;
        pipeline_shader_stage_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        pipeline_shader_stage_create_info[0].module = g_vertex_shader_module;
        pipeline_shader_stage_create_info[0].pName = "main";
        pipeline_shader_stage_create_info[0].pSpecializationInfo = NULL;

        pipeline_shader_stage_create_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipeline_shader_stage_create_info[1].pNext = NULL;
        pipeline_shader_stage_create_info[1].flags = 0;
        pipeline_shader_stage_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        pipeline_shader_stage_create_info[1].module = g_fragment_shader_module;
        pipeline_shader_stage_create_info[1].pName = "main";
        pipeline_shader_stage_create_info[1].pSpecializationInfo = NULL;

        VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_info;
        pipeline_vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipeline_vertex_input_state_info.pNext = NULL;
        pipeline_vertex_input_state_info.flags = 0;
        pipeline_vertex_input_state_info.vertexBindingDescriptionCount = 0;
        pipeline_vertex_input_state_info.pVertexBindingDescriptions = NULL;
        pipeline_vertex_input_state_info.vertexAttributeDescriptionCount = 0;
        pipeline_vertex_input_state_info.pVertexAttributeDescriptions = NULL;

        VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_info;
        pipeline_input_assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipeline_input_assembly_state_info.pNext = NULL;
        pipeline_input_assembly_state_info.flags = 0;
        pipeline_input_assembly_state_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipeline_input_assembly_state_info.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) window_width;
        viewport.height = (float) window_height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset.x = 0.0f,
        scissor.offset.y = 0.0f;
        scissor.extent.width = window_width;
        scissor.extent.height = window_height;

        VkPipelineViewportStateCreateInfo pipeline_viewport_state_info;
        pipeline_viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pipeline_viewport_state_info.pNext = NULL;
        pipeline_viewport_state_info.flags = 0;
        pipeline_viewport_state_info.viewportCount = 1;
        pipeline_viewport_state_info.pViewports = &viewport;
        pipeline_viewport_state_info.scissorCount = 1;
        pipeline_viewport_state_info.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_info;
        pipeline_rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipeline_rasterization_state_info.pNext = NULL;
        pipeline_rasterization_state_info.flags = 0;
        pipeline_rasterization_state_info.depthClampEnable = VK_FALSE;
        pipeline_rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
        pipeline_rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
        pipeline_rasterization_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
        pipeline_rasterization_state_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        pipeline_rasterization_state_info.depthBiasEnable = VK_FALSE;
        pipeline_rasterization_state_info.depthBiasConstantFactor = 0.0f;
        pipeline_rasterization_state_info.depthBiasClamp = 0.0f;
        pipeline_rasterization_state_info.depthBiasSlopeFactor = 0.0f;
        pipeline_rasterization_state_info.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_info;
        pipeline_multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipeline_multisample_state_info.pNext = NULL;
        pipeline_multisample_state_info.flags = 0;
        pipeline_multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipeline_multisample_state_info.sampleShadingEnable = VK_FALSE;
        pipeline_multisample_state_info.minSampleShading = 1.0f;
        pipeline_multisample_state_info.pSampleMask = NULL;
        pipeline_multisample_state_info.alphaToCoverageEnable = VK_FALSE;
        pipeline_multisample_state_info.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state;
        pipeline_color_blend_attachment_state.blendEnable = VK_FALSE;
        pipeline_color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        pipeline_color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipeline_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
        pipeline_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pipeline_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipeline_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
        pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_info;
        pipeline_color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipeline_color_blend_state_info.pNext = NULL;
        pipeline_color_blend_state_info.flags = 0;
        pipeline_color_blend_state_info.logicOpEnable = VK_FALSE;
        pipeline_color_blend_state_info.logicOp = VK_LOGIC_OP_COPY;
        pipeline_color_blend_state_info.attachmentCount = 1;
        pipeline_color_blend_state_info.pAttachments = &pipeline_color_blend_attachment_state;
        pipeline_color_blend_state_info.blendConstants[0] = 0.0f;
        pipeline_color_blend_state_info.blendConstants[1] = 0.0f;
        pipeline_color_blend_state_info.blendConstants[2] = 0.0f;
        pipeline_color_blend_state_info.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipeline_layout_create_info;
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_create_info.pNext = NULL;
        pipeline_layout_create_info.flags = 0;
        pipeline_layout_create_info.setLayoutCount = 0;
        pipeline_layout_create_info.pSetLayouts = NULL;
        pipeline_layout_create_info.pushConstantRangeCount = 0;
        pipeline_layout_create_info.pPushConstantRanges = NULL;

        if(vk_ctx->create_pipeline_layout(vk_ctx->device, &pipeline_layout_create_info, vk_ctx->allocation_callbacks, &g_pipeline_layout) != VK_SUCCESS)
        {
            printf("Could not create pipeline layout\n");
            status = false;
        }

        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info;
        graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_create_info.pNext = NULL;
        graphics_pipeline_create_info.flags = 0;
        graphics_pipeline_create_info.stageCount = 2;
        graphics_pipeline_create_info.pStages = pipeline_shader_stage_create_info;
        graphics_pipeline_create_info.pVertexInputState = &pipeline_vertex_input_state_info;
        graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_info;
        graphics_pipeline_create_info.pTessellationState = NULL;
        graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_info;
        graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterization_state_info;
        graphics_pipeline_create_info.pMultisampleState = &pipeline_multisample_state_info;
        graphics_pipeline_create_info.pDepthStencilState = NULL;
        graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_info;
        graphics_pipeline_create_info.pDynamicState = NULL;
        graphics_pipeline_create_info.layout = g_pipeline_layout;
        graphics_pipeline_create_info.renderPass = g_render_pass;
        graphics_pipeline_create_info.subpass = 0;
        graphics_pipeline_create_info.basePipelineHandle = NULL;
        graphics_pipeline_create_info.basePipelineIndex = -1;

        if(vk_ctx->create_graphics_pipelines(vk_ctx->device, NULL, 1, &graphics_pipeline_create_info, vk_ctx->allocation_callbacks, &g_graphics_pipeline) != VK_SUCCESS)
        {
            printf("Could not create graphics pipeline\n");
            status = false;
        }
    }

    if(ext_array != NULL)
    {
        free(ext_array);
        ext_array = NULL;
    }

    return status;
}

void uninitialize(void)
{
    uninitialize_vulkan_context();

    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();
}

bool render(void)
{
    bool status = true;

    uint32_t swapchain_index = 0;

    if(status)
    {
        if(vk_ctx->acquire_next_image(vk_ctx->device, vk_ctx->swapchain, UINT64_MAX, vk_ctx->image_available_semaphore, NULL, &swapchain_index) != VK_SUCCESS)
        {
            printf("Could not get next surface image\n");
            status = false;
        }
    }

    if(status)
    {
        VkCommandBufferBeginInfo params;
        params.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        params.pNext = NULL;
        params.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        params.pInheritanceInfo = NULL;

        if(vk_ctx->begin_command_buffer(vk_ctx->command_buffer, &params) != VK_SUCCESS)
        {
            printf("Failed to begin command buffer\n");
            status = false;
        }
    }

    if(status)
    {
        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = NULL;
        barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = vk_ctx->graphics_queue_family;
        barrier.dstQueueFamilyIndex = vk_ctx->graphics_queue_family;
        barrier.image = vk_ctx->swapchain_images[swapchain_index];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0,
        barrier.subresourceRange.layerCount = 1;

        vk_ctx->cmd_pipeline_barrier(vk_ctx->command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
    }

    if(status)
    {
        VkClearColorValue color;
        color.float32[0] = 0.0f;
        color.float32[1] = 0.0f;
        color.float32[2] = 1.0f;
        color.float32[3] = 0.0f;

        VkImageSubresourceRange subresource_range;
        subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource_range.baseMipLevel = 0;
        subresource_range.levelCount = 1;
        subresource_range.baseArrayLayer = 0,
        subresource_range.layerCount = 1;

        vk_ctx->cmd_clear_color_image(vk_ctx->command_buffer, vk_ctx->swapchain_images[swapchain_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &subresource_range);
    }

    if(status)
    {
        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = NULL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcQueueFamilyIndex = vk_ctx->graphics_queue_family;
        barrier.dstQueueFamilyIndex = vk_ctx->graphics_queue_family;
        barrier.image = vk_ctx->swapchain_images[swapchain_index];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0,
        barrier.subresourceRange.layerCount = 1;

        vk_ctx->cmd_pipeline_barrier(vk_ctx->command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
    }

    if(status)
    {
        if(vk_ctx->end_command_buffer(vk_ctx->command_buffer) != VK_SUCCESS)
        {
            printf("Failed to end command buffer\n");
            status = false;
        }
    }

    if(status)
    {
        VkPipelineStageFlags wait_dst_stage_masks[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };

        VkSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = NULL;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &vk_ctx->image_available_semaphore;
        submit_info.pWaitDstStageMask = wait_dst_stage_masks;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &vk_ctx->command_buffer;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &vk_ctx->rendering_finished_semaphore;

        if(vk_ctx->queue_submit(vk_ctx->graphics_queues[0], 1, &submit_info, NULL) != VK_SUCCESS)
        {
            printf("Failed to submit command buffer\n");
            status = false;
        }
    }

    if(status)
    {
        VkPresentInfoKHR present_info;
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = NULL;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &vk_ctx->rendering_finished_semaphore;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &vk_ctx->swapchain;
        present_info.pImageIndices = &swapchain_index;
        present_info.pResults = NULL;

        if(vk_ctx->queue_present(vk_ctx->graphics_queues[0], &present_info) != VK_SUCCESS)
        {
            printf("Failed to present\n");
            status = false;
        }
    }

    return status;
}

bool run(void)
{
    bool status = true;
    bool exit = false;
    SDL_Event event = { 0 };

    while(status)
    {
        while(status && (SDL_PollEvent(&event) != 0))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    exit = true;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        if(!exit)
        {
            status = render();
        }
        else
        {
            break;
        }
    }

    return status;
}

int main(int argc, char* argv[])
{
    int status = 0;

    if(!initialize())
    {
        status = -1;
    }

    if(status == 0)
    {
        if(!run())
        {
            status = -1;
        }
    }

    uninitialize();

    printf("Exit with code 0x%X\n", status);

    return status;
}
