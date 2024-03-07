
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "vk_context.h"

const char* window_title = "vk-cube";
const uint32_t window_width = 1024;
const uint32_t window_height = 768;

SDL_Window* g_window = NULL;

bool initialize(void)
{
    bool status = true;

    uint32_t ext_count = 0;
    const char** ext_array = NULL;

    vk_surface surface = NULL;

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
        if(vk_ctx->acquire_next_image(vk_ctx->device, vk_ctx->swapchain, UINT64_MAX, vk_ctx->image_available_semaphore, NULL, &swapchain_index) != vk_success)
        {
            printf("Could not get next surface image\n");
            status = false;
        }
    }

    if(status)
    {
        struct vk_command_buffer_begin_params params;
        params.s_type = vk_structure_type__command_buffer_begin_info;
        params.p_next = NULL;
        params.usage_flags = vk_command_buffer_usage_flag__one_time_submit;
        params.p_inheritance_info = NULL;

        if(vk_ctx->begin_command_buffer(vk_ctx->command_buffer, &params) != vk_success)
        {
            printf("Failed to begin command buffer\n");
            status = false;
        }
    }

    if(status)
    {
        struct vk_image_memory_barrier barrier;
        barrier.s_type = vk_structure_type__image_memory_barrier;
        barrier.p_next = NULL;
        barrier.src_access_mask = vk_access_flag__memory_read;
        barrier.dst_access_mask = vk_access_flag__transfer_write;
        barrier.old_layout = vk_image_layout__undefined;
        barrier.new_layout = vk_image_layout__transfer_dst_optimal;
        barrier.src_queue_family_index = vk_ctx->graphics_queue_family;
        barrier.dst_queue_family_index = vk_ctx->graphics_queue_family;
        barrier.image = vk_ctx->swapchain_images[swapchain_index];
        barrier.subresource_range.aspect_mask = vk_image_aspect__color;
        barrier.subresource_range.base_mip_level = 0;
        barrier.subresource_range.level_count = 1;
        barrier.subresource_range.base_array_layer = 0,
        barrier.subresource_range.layer_count = 1;

        vk_ctx->cmd_pipeline_barrier(vk_ctx->command_buffer, vk_pipeline_stage_flag__transfer, vk_pipeline_stage_flag__transfer, 0, 0, NULL, 0, NULL, 1, &barrier);
    }

    if(status)
    {
        struct vk_clear_color color;
        color.float32[0] = 0.0f;
        color.float32[1] = 0.0f;
        color.float32[2] = 1.0f;
        color.float32[3] = 0.0f;

        struct vk_image_subresource_range subresource_range;
        subresource_range.aspect_mask = vk_image_aspect__color;
        subresource_range.base_mip_level = 0;
        subresource_range.level_count = 1;
        subresource_range.base_array_layer = 0,
        subresource_range.layer_count = 1;

        vk_ctx->cmd_clear_color_image(vk_ctx->command_buffer, vk_ctx->swapchain_images[swapchain_index], vk_image_layout__transfer_dst_optimal, &color, 1, &subresource_range);
    }

    if(status)
    {
        struct vk_image_memory_barrier barrier;
        barrier.s_type = vk_structure_type__image_memory_barrier;
        barrier.p_next = NULL;
        barrier.src_access_mask = vk_access_flag__transfer_write;
        barrier.dst_access_mask = vk_access_flag__memory_read;
        barrier.old_layout = vk_image_layout__transfer_dst_optimal;
        barrier.new_layout = vk_image_layout__present_src_khr;
        barrier.src_queue_family_index = vk_ctx->graphics_queue_family;
        barrier.dst_queue_family_index = vk_ctx->graphics_queue_family;
        barrier.image = vk_ctx->swapchain_images[swapchain_index];
        barrier.subresource_range.aspect_mask = vk_image_aspect__color;
        barrier.subresource_range.base_mip_level = 0;
        barrier.subresource_range.level_count = 1;
        barrier.subresource_range.base_array_layer = 0,
        barrier.subresource_range.layer_count = 1;

        vk_ctx->cmd_pipeline_barrier(vk_ctx->command_buffer, vk_pipeline_stage_flag__transfer, vk_pipeline_stage_flag__transfer, 0, 0, NULL, 0, NULL, 1, &barrier);
    }

    if(status)
    {
        if(vk_ctx->end_command_buffer(vk_ctx->command_buffer) != vk_success)
        {
            printf("Failed to end command buffer\n");
            status = false;
        }
    }

    if(status)
    {
        enum vk_pipeline_stage_flags wait_dst_stage_masks[] = { vk_pipeline_stage_flag__transfer };

        struct vk_submission_info submit_info;
        submit_info.s_type = vk_structure_type__submit_info;
        submit_info.p_next = NULL;
        submit_info.wait_semaphore_count = 1;
        submit_info.wait_semaphores = &vk_ctx->image_available_semaphore;
        submit_info.wait_dst_stage_masks = wait_dst_stage_masks;
        submit_info.command_buffer_count = 1;
        submit_info.command_buffers = &vk_ctx->command_buffer;
        submit_info.signal_semaphore_count = 1;
        submit_info.signal_semahores = &vk_ctx->rendering_finished_semaphore;

        if(vk_ctx->queue_submit(vk_ctx->graphics_queues[0], 1, &submit_info, NULL) != vk_success)
        {
            printf("Failed to submit command buffer\n");
            status = false;
        }
    }

    if(status)
    {
        struct vk_present_info present_info;
        present_info.s_type = vk_structure_type__present_info;
        present_info.p_next = NULL;
        present_info.wait_semaphore_count = 1;
        present_info.wait_semaphores = &vk_ctx->rendering_finished_semaphore;
        present_info.swapchain_count = 1;
        present_info.swapchains = &vk_ctx->swapchain;
        present_info.image_indices = &swapchain_index;
        present_info.results = NULL;

        if(vk_ctx->queue_present(vk_ctx->graphics_queues[0], &present_info) != vk_success)
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

            /*
            if(status)
            {
                status = present();
            }
            */
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
