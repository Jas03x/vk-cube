
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "vk_context.h"

struct vk_context  g_vk_ctx = { 0 };
struct vk_context* vk_ctx   = &g_vk_ctx;

VkAllocationCallbacks g_allocation_callbacks = { 0 };

struct vk_memory_usage
{
    uint64_t size;
} g_vk_memory_usage;

struct vk_allocation_header
{
    uint64_t size;
    uint64_t reserved[3];
};

enum { MAX_EXTENSIONS = 16 };
enum { INVALID_INDEX  = 0xFFFFFFFF };

struct gpu_info
{
    VkPhysicalDevice                     handle;
    VkPhysicalDeviceProperties           properties;
    VkPhysicalDeviceFeatures             features;

    uint32_t                             queue_group_count;
    VkQueueFamilyProperties*             queue_group_properties;
};

struct extension_list
{
    uint32_t               count;
    VkExtensionProperties* array;
};

struct layer_list
{
    uint32_t               count;
    VkLayerProperties*     array;

    struct extension_list* extension_lists;
};

bool     initialize_global_function_pointers(void);
bool     initialize_instance_function_pointers(void);
bool     initialize_device_function_pointers(void);
bool     initialize_allocation_callbacks(void);

bool     initialize_instance(uint32_t ext_count, const char** ext_array);
bool     initialize_device(void);
bool     initialize_queues(void);
bool     initialize_debug_layer(void);

bool     enumerate_instance_layers(struct layer_list* instance_layers);
bool     enumerate_instance_extensions(const char* layer, struct extension_list* instance_extensions);
bool     enumerate_gpus(uint32_t* gpu_count, struct gpu_info** gpu_info_array);
bool     enumerate_device_layers_and_extensions(VkPhysicalDevice physical_device, struct layer_list* device_layers);
bool     enumerate_device_extensions(VkPhysicalDevice physical_device, const char* layer, struct extension_list* device_extensions);

bool     get_gpu_queue_info(VkPhysicalDevice physical_device, uint32_t* num_queue_groups, struct VkQueueFamilyProperties** queue_group_properties);

void     print_gpu_info(uint32_t gpu_index, struct gpu_info* gpu_info);

uint32_t find_extension(struct extension_list* extension_list, const char* extension_name);
bool     add_extension(struct extension_list* extensions, const char** ext_array, uint32_t* ext_count, const char* ext_name);

void     free_layers(struct layer_list* layers);
void     free_extensions(struct extension_list* extensions);
void     free_gpu_info(uint32_t gpu_count, struct gpu_info* gpu_info_array);

void* vk_allocation_callback(void* user_data, uint64_t size, uint64_t alignment, enum vk_system_allocation_scope scope)
{
    uint64_t aligned_size = size + (alignment-1) & ~(alignment-1);
    uint64_t total_size = sizeof(struct vk_allocation_header) + aligned_size;

    uint8_t* memory = (uint8_t*) malloc(total_size);

    if(memory != NULL)
    {
        struct vk_allocation_header* header = (struct vk_allocation_header*) memory;

        memset(memory, 0, total_size);
        header->size = aligned_size;
        g_vk_memory_usage.size += aligned_size;
    }

    return memory + sizeof(struct vk_allocation_header);
}

void* vk_reallocation_callback(void* user_data, void* original, uint64_t size, uint64_t alignment, enum vk_system_allocation_scope scope)
{
    uint8_t* original_ptr = ((uint8_t*) original) - sizeof(struct vk_allocation_header);
    uint64_t original_size = ((struct vk_allocation_header*) original_ptr)->size;

    uint64_t aligned_size = size + (alignment-1) & ~(alignment-1);
    uint64_t total_size = sizeof(struct vk_allocation_header) + aligned_size;

    uint8_t* memory = (uint8_t*) realloc(original_ptr, total_size);

    if(memory != NULL)
    {
        struct vk_allocation_header* header = (struct vk_allocation_header*) memory;

        header->size = aligned_size;
        g_vk_memory_usage.size += (aligned_size - original_size);
    }

    return memory + sizeof(struct vk_allocation_header);
}

void vk_free_callback(void* user_data, void* allocation)
{
    uint8_t* original_ptr = ((uint8_t*) allocation) - sizeof(struct vk_allocation_header);
    struct vk_allocation_header* header = (struct vk_allocation_header*) original_ptr;

    if(header->size <= g_vk_memory_usage.size)
    {
        g_vk_memory_usage.size -= header->size;
    }
    else
    {
        printf("Memory over-freed\n");
    }

    free(original_ptr);
}

void vk_allocation_notification(void* user_data, uint64_t size, enum vk_internal_allocation_type type, enum vk_system_allocation_scope scope)
{
}

void vk_free_notification(void* user_data, uint64_t size, enum vk_internal_allocation_type type, enum vk_system_allocation_scope scope)
{
}

VkBool32 debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT object_type, uint64_t object, uint64_t location, int32_t message_code, const char* layer_prefix, const char* message, void* user_data)
{
    bool status = true;

    enum { buffer_size = 512 };

    uint32_t index = 0;
    char buffer[buffer_size] = { 0 };

    if(status && (index < buffer_size))
    {
        if(flags == VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
        {
            index += snprintf(buffer + index, buffer_size - index, "Information: ");
        }
        else if(flags == VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            index += snprintf(buffer + index, buffer_size - index, "Warning: ");
        }
        else if(flags == VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        {
            index += snprintf(buffer + index, buffer_size - index, "Performance Warning: ");
        }
        else if(flags == VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            index += snprintf(buffer + index, buffer_size - index, "Error: ");
        }
        else if(flags == VK_DEBUG_REPORT_DEBUG_BIT_EXT)
        {
            index += snprintf(buffer + index, buffer_size - index, "Debug: ");
        }
        else
        {
            index += snprintf(buffer + index, buffer_size - index, "Unknown: ");
        }
    }
    else
    {
        status = false;
    }

    if(status && (index < buffer_size))
    {
        switch(object_type)
        {
            case VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Instance ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:PhysicalDevice ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Device ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Queue ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Semaphore ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:CommandBuffer ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Fence ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:DeviceMemory ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Buffer ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Image ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Event ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:QueryPool ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:BufferView ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:ImageView ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:ShaderModule ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:PipelineCache ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:PipelineLayout ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:RenderPass ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Pipeline ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:DescriptorSetLayout ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Sampler ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:DescriptorPool ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:DescriptorSet ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Framebuffer ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:CommandPool ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Surface ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Swapchain ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:DebugReport ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Display ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:DisplayMode ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:ValidationCache ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:Conversion ");
                break;
            case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_EXT:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:DescriptorUpdateTemplate ");
                break;
            default:
                index += snprintf(buffer + index, buffer_size - index, "ObjectType:0x%X ", object_type);
                break;
        }
    }
    else
    {
        status = false;
    }

    if(status && (index < buffer_size)) { index += snprintf(buffer + index, buffer_size - index, "Object:0x%llX ", object); } else { status = false; }
    if(status && (index < buffer_size)) { index += snprintf(buffer + index, buffer_size - index, "Location:0x%llX ", location); } else { status = false; }
    if(status && (index < buffer_size)) { index += snprintf(buffer + index, buffer_size - index, "MessageCode:0x%X ", message_code); } else { status = false; }
    if(status && (index < buffer_size)) { index += snprintf(buffer + index, buffer_size - index, "Layer:%s ", layer_prefix); } else { status = false; }
    if(status && (index < buffer_size)) { index += snprintf(buffer + index, buffer_size - index, "UserData:0x%llX\n", (uint64_t) user_data); } else { status = false; }
    if(status && (index < buffer_size)) { index += snprintf(buffer + index, buffer_size - index, "\t%s", message); } else { status = false; }

    if (index >= buffer_size)
    {
        status = false; // ran out of buffer space
    }

    if(status)
    {
        printf("%s\n", buffer);
    }
    else
    {
        printf("Error: Could not parse debug report message\n");
    }

    return VK_TRUE;
}

bool initialize_vulkan_context(PFN_vkGetInstanceProcAddr pfn_get_instance_proc_addr, uint32_t ext_count, const char** ext_array)
{
    bool status = true;

    g_vk_ctx.get_instance_proc_addr = pfn_get_instance_proc_addr;

    if(status)
    {
        status = initialize_global_function_pointers();
    }

    if(status)
    {
        status = initialize_allocation_callbacks();
    }

    if(status)
    {
        status = initialize_instance(ext_count, ext_array);
    }

    if(status)
    {
        status = initialize_device();
    }

    if(status)
    {
        status = initialize_queues();
    }

    return status;
}

void uninitialize_vulkan_context(void)
{
    g_vk_ctx.wait_for_device_idle(g_vk_ctx.device);

    if(g_vk_ctx.command_buffer != NULL)
    {
        g_vk_ctx.free_command_buffers(g_vk_ctx.device, g_vk_ctx.command_pool, 1, &g_vk_ctx.command_buffer);
        g_vk_ctx.command_buffer = NULL;
    }

    if(g_vk_ctx.command_pool != NULL)
    {
        g_vk_ctx.destroy_command_pool(g_vk_ctx.device, g_vk_ctx.command_pool, g_vk_ctx.allocation_callbacks);
        g_vk_ctx.command_pool = NULL;
    }

    if(g_vk_ctx.surface != NULL)
    {
        g_vk_ctx.destroy_surface(g_vk_ctx.instance, g_vk_ctx.surface, NULL);
        g_vk_ctx.surface = NULL;
    }

    if(g_vk_ctx.image_available_semaphore != NULL)
    {
        g_vk_ctx.destroy_semaphore(g_vk_ctx.device, g_vk_ctx.image_available_semaphore, g_vk_ctx.allocation_callbacks);
        g_vk_ctx.image_available_semaphore = NULL;
    }

    if(g_vk_ctx.rendering_finished_semaphore != NULL)
    {
        g_vk_ctx.destroy_semaphore(g_vk_ctx.device, g_vk_ctx.rendering_finished_semaphore, g_vk_ctx.allocation_callbacks);
        g_vk_ctx.rendering_finished_semaphore = NULL;
    }

    if(g_vk_ctx.swapchain != NULL)
    {
        g_vk_ctx.destroy_swapchain(g_vk_ctx.device, g_vk_ctx.swapchain, g_vk_ctx.allocation_callbacks);
        g_vk_ctx.swapchain = NULL;
    }

    g_vk_ctx.destroy_device(g_vk_ctx.device, g_vk_ctx.allocation_callbacks);
    g_vk_ctx.device = NULL;

    if(g_vk_ctx.debug_callback != NULL)
    {
        g_vk_ctx.unregister_debug_callback(g_vk_ctx.instance, g_vk_ctx.debug_callback, g_vk_ctx.allocation_callbacks);
    }

    g_vk_ctx.destroy_instance(g_vk_ctx.instance, g_vk_ctx.allocation_callbacks);
    g_vk_ctx.instance = NULL;

    memset(&g_vk_ctx, 0, sizeof(struct vk_context));

    if(g_vk_memory_usage.size != 0)
    {
        printf("Critical error: VK leaking %llu bytes\n", g_vk_memory_usage.size);
    }
}

bool initialize_queues(void)
{
    bool status = true;

    if(status)
    {
        for(uint32_t i = 0; i < VK_CTX_NUM_GRAPHICS_QUEUES; i++)
        {
            g_vk_ctx.get_device_queue(g_vk_ctx.device, g_vk_ctx.graphics_queue_family, i, g_vk_ctx.graphics_queues);

            if(g_vk_ctx.graphics_queues[i] == NULL)
            {
                status = false;
                printf("Could not get graphics queue %u\n", i);
                break;
            }
        }
    }

    if(status)
    {
        VkCommandPoolCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = NULL;
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = g_vk_ctx.graphics_queue_family;

        if(g_vk_ctx.create_command_pool(g_vk_ctx.device, &info, g_vk_ctx.allocation_callbacks, &g_vk_ctx.command_pool) != VK_SUCCESS)
        {
            printf("Failed to create command pool\n");
            status = false;
        }
    }

    if(status)
    {
        VkCommandBufferAllocateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = NULL;
        info.commandPool = g_vk_ctx.command_pool;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;

        if(g_vk_ctx.allocate_command_buffers(g_vk_ctx.device, &info, &g_vk_ctx.command_buffer) != VK_SUCCESS)
        {
            printf("Failed to create command buffer\n");
            status = false;
        }
    }

    return status;
}

bool initialize_swapchain(VkSurfaceKHR surface)
{
    bool status = true;

    uint32_t supported = VK_FALSE;
    uint32_t format_count = 0;
    uint32_t present_mode_count = 0;
    uint32_t num_swapchain_images = 0;
    uint32_t format_index = INVALID_INDEX;
    
    VkPresentModeKHR* present_mode_array = NULL;
    VkSurfaceFormatKHR* format_array = NULL;

    VkSurfaceCapabilitiesKHR surface_capabilities = { 0 };

    if(surface != NULL)
    {
        g_vk_ctx.surface = surface;
    }
    else
    {
        printf("Invalid surface");
        status = false;
    }

    if(status)
    {
        if(g_vk_ctx.get_physical_device_surface_support(g_vk_ctx.physical_device, g_vk_ctx.graphics_queue_family, surface, &supported) != VK_SUCCESS)
        {
            printf("Failed to check device surface support\n");
            status = false;
        }
    }

    if(status)
    {
        if(supported != VK_TRUE)
        {
            printf("Device/graphics queue family does not support surface\n");
            status = false;
        }
    }

    if(status)
    {
        if(g_vk_ctx.get_physical_device_surface_capabilities(g_vk_ctx.physical_device, surface, &surface_capabilities) != VK_SUCCESS)
        {
            printf("Could not get surface capabilities\n");
            status = false;
        }
    }

    if(status)
    {
        if(g_vk_ctx.get_physical_device_surface_present_modes(g_vk_ctx.physical_device, surface, &present_mode_count, NULL) != VK_SUCCESS)
        {
            printf("Could not get present mode count\n");
            status = false;
        }
    }

    if(status)
    {
        if(g_vk_ctx.get_physical_device_surface_formats(g_vk_ctx.physical_device, surface, &format_count, NULL) != VK_SUCCESS)
        {
            printf("Count not get format count\n");
            status = false;
        }
    }

    if(status)
    {
        present_mode_array = (VkPresentModeKHR*) malloc(present_mode_count * sizeof(VkPresentModeKHR));
        if(present_mode_array == NULL)
        {
            printf("Failed to allocate memory for present mode array\n");
            status = false;
        }
    }

    if(status)
    {
        format_array = (VkSurfaceFormatKHR*) malloc(format_count * sizeof(VkSurfaceFormatKHR));
        if(format_array == NULL)
        {
            printf("Failed to allocate memory for format array\n");
            status = false;
        }
    }

    if(status)
    {
        if(g_vk_ctx.get_physical_device_surface_present_modes(g_vk_ctx.physical_device, surface, &present_mode_count, present_mode_array) != VK_SUCCESS)
        {
            printf("Could not get present mode array\n");
            status = false;
        }
    }

    if(status)
    {
        if(g_vk_ctx.get_physical_device_surface_formats(g_vk_ctx.physical_device, surface, &format_count, format_array) != VK_SUCCESS)
        {
            printf("Could not get format array");
            status = false;
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < format_count; i++)
        {
            if(format_array[i].format == VK_FORMAT_R8G8B8A8_UNORM)
            {
                format_index = i;
                break;
            }
        }

        if(format_index >= format_count)
        {
            printf("Could not find supported format\n");
            status = false;
        }
    }

    if(status)
    {
        printf("Surface capabilities:\n");
        printf("\tMinimum image count: %u\n", surface_capabilities.minImageCount);
        printf("\tMaximum image count: %u\n", surface_capabilities.maxImageCount);
        printf("\tCurrent extent: %ux%u\n", surface_capabilities.currentExtent.width, surface_capabilities.currentExtent.height);
        printf("\tMinimum extent: %ux%u\n", surface_capabilities.minImageExtent.width, surface_capabilities.minImageExtent.height);
        printf("\tMaximum extent: %ux%u\n", surface_capabilities.maxImageExtent.width, surface_capabilities.maxImageExtent.height);
        printf("\tMaximum array image array layers: %u\n", surface_capabilities.maxImageArrayLayers);
        printf("\tSupported transforms: %u\n", surface_capabilities.supportedTransforms);
        printf("\tCurrent transform: %u\n", surface_capabilities.currentTransform);
        printf("\tSupported composite alpha: %u\n", surface_capabilities.supportedCompositeAlpha);
        printf("\tSupported usage flags: %u\n", surface_capabilities.supportedUsageFlags);

        printf("Surface supported present modes:\n");
        for(uint32_t i = 0; i < present_mode_count; i++)
        {
            const char* mode = "Unknown";
            switch(present_mode_array[i])
            {
                case VK_PRESENT_MODE_IMMEDIATE_KHR:
                    mode = "Immediate";
                    break;
                case VK_PRESENT_MODE_MAILBOX_KHR:
                    mode = "Mailbox";
                    break;
                case VK_PRESENT_MODE_FIFO_KHR:
                    mode = "FIFO";
                    break;
                case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
                    mode = "FIFO relaxed";
                    break;
                case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
                    mode = "Demand refresh";
                    break;
                case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
                    mode = "Continuous refresh";
                    break;
                default:
                    break;
            }
            printf("\t%s\n", mode);
        }
    }

    if(status)
    {
        VkSwapchainCreateInfoKHR info;
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext = NULL;
        info.flags = 0;
        info.surface = surface;
        info.minImageCount = VK_CTX_NUM_SWAPCHAIN_BUFFERS;
        info.imageFormat = format_array[format_index].format;
        info.imageColorSpace = format_array[format_index].colorSpace;
        info.imageExtent.width = surface_capabilities.currentExtent.width;
        info.imageExtent.height = surface_capabilities.currentExtent.height;
        info.imageArrayLayers = 1;
        info.imageUsage = 0;
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = NULL;
        info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        info.clipped = VK_TRUE;
        info.oldSwapchain = NULL;

        if(g_vk_ctx.create_swapchain(g_vk_ctx.device, &info, g_vk_ctx.allocation_callbacks, &g_vk_ctx.swapchain) != VK_SUCCESS)
        {
            printf("Could not create swapchain\n");
            status = false;
        }
    }

    if(status)
    {
        g_vk_ctx.surface_format = format_array[format_index].format;
    }

    if(status)
    {
        VkSemaphoreCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;

        if(g_vk_ctx.create_semaphore(g_vk_ctx.device, &info, g_vk_ctx.allocation_callbacks, &g_vk_ctx.image_available_semaphore) != VK_SUCCESS)
        {
            printf("Failed to create semaphore\n");
            status = false;
        }
    }

    if(status)
    {
        VkSemaphoreCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;

        if(g_vk_ctx.create_semaphore(g_vk_ctx.device, &info, g_vk_ctx.allocation_callbacks, &g_vk_ctx.rendering_finished_semaphore) != VK_SUCCESS)
        {
            printf("Failed to create semaphore\n");
            status = false;
        }
    }

    if(status)
    {
        if(g_vk_ctx.get_swapchain_images(g_vk_ctx.device, g_vk_ctx.swapchain, &num_swapchain_images, NULL) == VK_SUCCESS)
        {
            if(num_swapchain_images != VK_CTX_NUM_SWAPCHAIN_BUFFERS)
            {
                printf("Unexpected number of swapchain images\n");
                status = false;
            }
        }
        else
        {
            printf("Could not get swapchain image count\n");
            status = false;
        }
    }

    if(status)
    {
        if(g_vk_ctx.get_swapchain_images(g_vk_ctx.device, g_vk_ctx.swapchain, &num_swapchain_images, g_vk_ctx.swapchain_images) != VK_SUCCESS)
        {
            printf("Could not get swapchain images\n");
            status = false;
        }
    }

    if(format_array != NULL)
    {
        free(format_array);
        format_array = NULL;
    }

    if(present_mode_array != NULL)
    {
        free(present_mode_array);
        present_mode_array = NULL;
    }

    return status;
}

bool load_function_pointer(VkInstance instance, const char* name, void** pfn)
{
    bool status = true;

    if(pfn != NULL)
    {
        (*pfn) = g_vk_ctx.get_instance_proc_addr(instance, name);

        if((*pfn) == NULL)
        {
            status = false;
            printf("Failed to load function pointer %s\n", name);
        }
    }
    else
    {
        status = false;
    }

    return status;
}

bool load_device_function_pointer(VkDevice device, const char* name, void** pfn)
{
    bool status = true;

    if(pfn != NULL)
    {
        (*pfn) = g_vk_ctx.get_device_proc_addr(device, name);

        if((*pfn) == NULL)
        {
            status = false;
            printf("Failed to load device function pointer %s\n", name);
        }
    }
    else
    {
        status = false;
    }

    return status;
}

bool initialize_global_function_pointers(void)
{
    bool status = true;

    status &= load_function_pointer(NULL, "vkCreateInstance", (void**) &g_vk_ctx.create_instance);
    status &= load_function_pointer(NULL, "vkEnumerateInstanceVersion", (void**) &g_vk_ctx.enumerate_instance_version);
    status &= load_function_pointer(NULL, "vkEnumerateInstanceLayerProperties", (void**) &g_vk_ctx.enumerate_instance_layers);
    status &= load_function_pointer(NULL, "vkEnumerateInstanceExtensionProperties", (void**) &g_vk_ctx.enumerate_instance_extensions);

    return status;
}

bool initialize_instance_function_pointers(void)
{
    bool status = true;

    status &= load_function_pointer(g_vk_ctx.instance, "vkDestroyInstance", (void**) &g_vk_ctx.destroy_instance);
    status &= load_function_pointer(g_vk_ctx.instance, "vkEnumeratePhysicalDevices", (void**) &g_vk_ctx.enumerate_physical_devices);
    status &= load_function_pointer(g_vk_ctx.instance, "vkGetPhysicalDeviceProperties", (void**) &g_vk_ctx.get_physical_device_properties);
    status &= load_function_pointer(g_vk_ctx.instance, "vkGetPhysicalDeviceFeatures", (void**) &g_vk_ctx.get_physical_device_features);
    status &= load_function_pointer(g_vk_ctx.instance, "vkGetPhysicalDeviceQueueFamilyProperties", (void**) &g_vk_ctx.get_physical_queue_group_properties);
    status &= load_function_pointer(g_vk_ctx.instance, "vkCreateDevice", (void**) &g_vk_ctx.create_device);
    status &= load_function_pointer(g_vk_ctx.instance, "vkGetPhysicalDeviceSurfaceSupportKHR", (void**) &g_vk_ctx.get_physical_device_surface_support);
    status &= load_function_pointer(g_vk_ctx.instance, "vkEnumerateDeviceLayerProperties", (void**) &g_vk_ctx.enumerate_device_layers);
    status &= load_function_pointer(g_vk_ctx.instance, "vkEnumerateDeviceExtensionProperties", (void**) &g_vk_ctx.enumerate_device_extensions);
    status &= load_function_pointer(g_vk_ctx.instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR", (void**) &g_vk_ctx.get_physical_device_surface_capabilities);
    status &= load_function_pointer(g_vk_ctx.instance, "vkGetPhysicalDeviceSurfacePresentModesKHR", (void**) &g_vk_ctx.get_physical_device_surface_present_modes);
    status &= load_function_pointer(g_vk_ctx.instance, "vkGetPhysicalDeviceSurfaceFormatsKHR", (void**) &g_vk_ctx.get_physical_device_surface_formats);
    status &= load_function_pointer(g_vk_ctx.instance, "vkDestroySurfaceKHR", (void**) &g_vk_ctx.destroy_surface);

#ifdef DEBUG
    status &= load_function_pointer(g_vk_ctx.instance, "vkCreateDebugReportCallbackEXT", (void**) &g_vk_ctx.register_debug_callback);
    status &= load_function_pointer(g_vk_ctx.instance, "vkDestroyDebugReportCallbackEXT", (void*) &g_vk_ctx.unregister_debug_callback);
#endif

    return status;
}

bool initialize_device_function_pointers(void)
{
    bool status = true;

    status &= load_function_pointer(g_vk_ctx.instance, "vkGetDeviceProcAddr", (void**) &g_vk_ctx.get_device_proc_addr);

    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreateSemaphore", (void**) &g_vk_ctx.create_semaphore);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkDestroySemaphore", (void**) &g_vk_ctx.destroy_semaphore);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreateSwapchainKHR", (void**) &g_vk_ctx.create_swapchain);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkDestroySwapchainKHR", (void**) &g_vk_ctx.destroy_swapchain);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkAcquireNextImageKHR", (void**) &g_vk_ctx.acquire_next_image);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkDeviceWaitIdle", (void**) &g_vk_ctx.wait_for_device_idle);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkDestroyDevice", (void**) &g_vk_ctx.destroy_device);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreateCommandPool", (void**) &g_vk_ctx.create_command_pool);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkDestroyCommandPool", (void**) &g_vk_ctx.destroy_command_pool);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkAllocateCommandBuffers", (void**) &g_vk_ctx.allocate_command_buffers);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkFreeCommandBuffers", (void**) &g_vk_ctx.free_command_buffers);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkBeginCommandBuffer", (void**) &g_vk_ctx.begin_command_buffer);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkEndCommandBuffer", (void**) &g_vk_ctx.end_command_buffer);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCmdPipelineBarrier", (void**) &g_vk_ctx.cmd_pipeline_barrier);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkGetSwapchainImagesKHR", (void**) &g_vk_ctx.get_swapchain_images);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCmdClearColorImage", (void**) &g_vk_ctx.cmd_clear_color_image);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkQueueSubmit", (void**) &g_vk_ctx.queue_submit);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkQueuePresentKHR", (void**) &g_vk_ctx.queue_present);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkGetDeviceQueue", (void**) &g_vk_ctx.get_device_queue);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreateRenderPass", (void**) &g_vk_ctx.create_render_pass);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreateImageView", (void**) &g_vk_ctx.create_image_view);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreateFramebuffer", (void**) &g_vk_ctx.create_framebuffer);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreateShaderModule", (void**) &g_vk_ctx.create_shader_module);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreatePipelineLayout", (void**) &g_vk_ctx.create_pipeline_layout);
    status &= load_device_function_pointer(g_vk_ctx.device, "vkCreateGraphicsPipelines", (void**) &g_vk_ctx.create_graphics_pipelines);

    return status;
}

bool initialize_allocation_callbacks(void)
{
    bool status = true;

    g_allocation_callbacks.pUserData = NULL;
    g_allocation_callbacks.pfnAllocation = vk_allocation_callback;
    g_allocation_callbacks.pfnReallocation = vk_reallocation_callback;
    g_allocation_callbacks.pfnFree = vk_free_callback;
    g_allocation_callbacks.pfnInternalAllocation = vk_allocation_notification;
    g_allocation_callbacks.pfnInternalFree = vk_free_notification;

    g_vk_ctx.allocation_callbacks = &g_allocation_callbacks;

    return status;
}

bool enumerate_instance_layers(struct layer_list* instance_layers)
{
    bool status = true;

    uint32_t num_layers = 0;
    struct layer_list layers = { 0 };

    if(g_vk_ctx.enumerate_instance_layers(&num_layers, NULL) == VK_SUCCESS)
    {
        layers.count = num_layers + 1; // +1 for vulkan implementation
    }
    else
    {
        status = false;
        printf("Failed to get number of layers\n");
    }

    if(status)
    {
        layers.array = calloc(layers.count, sizeof(VkLayerProperties));

        if(layers.array == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        layers.extension_lists = calloc(layers.count, sizeof(struct extension_list));

        if(layers.extension_lists == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        // enumerate the extensions provided by the vulkan implementation
        printf("Layer: Vulkan Implementation\n");
        status = enumerate_instance_extensions(NULL, &layers.extension_lists[0]);
    }

    if(status)
    {
        if(g_vk_ctx.enumerate_instance_layers(&num_layers, &layers.array[1]) != VK_SUCCESS) // start reading at element 1 because element 0 is the vulkan implementation
        {
            status = false;
            printf("Failed to get layers\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 1; i <= num_layers; i++) // start at 1 because index 0 used for vulkan implementation
        {
            printf("Layer %s (0x%X, %u): %s\n", layers.array[i].layerName, layers.array[i].specVersion, layers.array[i].implementationVersion, layers.array[i].description);
            status = enumerate_instance_extensions(layers.array[i].layerName, &layers.extension_lists[i]);
        }
    }

    if(status)
    {
        instance_layers->array = layers.array;
        instance_layers->count = layers.count;
        instance_layers->extension_lists = layers.extension_lists;
    }
    else
    {
        free_layers(&layers);
    }

    return status;
}

uint32_t find_extension(struct extension_list* extension_list, const char* extension_name)
{
    uint32_t index = INVALID_INDEX;

    for(uint32_t i = 0; i < extension_list->count; i++)
    {
        if(strncmp(extension_list->array[i].extensionName, extension_name, VK_MAX_EXTENSION_NAME_SIZE) == 0)
        {
            index = i;
        }
    }

    return index;
}

bool add_extension(struct extension_list* extensions, const char** ext_array, uint32_t* ext_count, const char* ext_name)
{
    bool status = true;
    uint32_t index = INVALID_INDEX;

    if((*ext_count) + 1 <= MAX_EXTENSIONS)
    {
        index = find_extension(extensions, ext_name);

        if(index != INVALID_INDEX)
        {
            ext_array[*ext_count] = extensions->array[index].extensionName;
            (*ext_count)++;
        }
        else
        {
            printf("Could not find extension %s\n", ext_name);
        }
    }
    else
    {
        printf("Extension list out of room\n");
        status = false;
    }

    return status;
}

void free_layers(struct layer_list* layers)
{
    if(layers != NULL)
    {
        if(layers->extension_lists != NULL)
        {
            for(uint32_t i = 0; i < layers->count; i++)
            {
                free_extensions(&layers->extension_lists[i]);
            }

            free(layers->extension_lists);
            layers->extension_lists = NULL;
        }

        if(layers->array != NULL)
        {
            free(layers->array);
            layers->array = NULL;
        }

        layers->count = 0;
    }
}

void free_extensions(struct extension_list* extensions)
{
    if(extensions != NULL)
    {
        if(extensions->array != NULL)
        {
            free(extensions->array);
            extensions->array = NULL;
        }

        extensions->count = 0;
    }
}

bool enumerate_instance_extensions(const char* layer, struct extension_list* instance_extensions)
{
    bool status = true;

    struct extension_list extensions = { 0 };

    if(g_vk_ctx.enumerate_instance_extensions(layer, &extensions.count, NULL) != VK_SUCCESS)
    {
        status = false;
        printf("Failed to get number of extensions\n");
    }

    if(status)
    {
        extensions.array = calloc(extensions.count, sizeof(VkExtensionProperties));

        if(extensions.array == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(g_vk_ctx.enumerate_instance_extensions(layer, &extensions.count, extensions.array) != VK_SUCCESS)
        {
            status = false;
            printf("Failed to get extensions\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < extensions.count; i++)
        {
            printf("\tExtension: %s\n", extensions.array[i].extensionName);
        }
    }

    if(status)
    {
        instance_extensions->array = extensions.array;
        instance_extensions->count = extensions.count;
    }
    else
    {
        free_extensions(&extensions);
    }

    return status;
}

bool initialize_instance(uint32_t ext_count, const char** ext_array)
{
    bool status = true;

    struct layer_list layers = { 0 };

    uint32_t num_extensions = 0;
    const char* extensions[MAX_EXTENSIONS] = { 0 };

    if(status)
    {
        status = enumerate_instance_layers(&layers);
    }

    if(status)
    {
        for(uint32_t i = 0; status && (i < ext_count); i++)
        {
            status = add_extension(&layers.extension_lists[0], extensions, &num_extensions, ext_array[i]);
        }

        if(!status)
        {
            printf("Could not enable all required extensions\n");
            status = false;
        }
    }

#ifdef DEBUG
    if(status)
    {
        status = add_extension(&layers.extension_lists[0], extensions, &num_extensions, "VK_EXT_debug_report");
    }
#endif

    if(status)
    {
        VkApplicationInfo app_info;
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pNext = NULL;
        app_info.pApplicationName = "vk-cube";
        app_info.applicationVersion = 1;
        app_info.pEngineName = "vk-cube";
        app_info.engineVersion = 1;
        app_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instance_info;
        instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_info.pNext = NULL;
        instance_info.flags = 0;
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledLayerCount = 0;
        instance_info.ppEnabledLayerNames = NULL;
        instance_info.enabledExtensionCount = num_extensions;
        instance_info.ppEnabledExtensionNames = extensions;

        if(g_vk_ctx.create_instance(&instance_info, g_vk_ctx.allocation_callbacks, &g_vk_ctx.instance) != VK_SUCCESS)
        {
            status = false;
            printf("Failed to create vulkan instance\n");
        }
    }

    if(status)
    {
        status = initialize_instance_function_pointers();
    }

#ifdef DEBUG
    if(status)
    {
        status = initialize_debug_layer();
    }
#endif

    free_layers(&layers);
    
    return status;
}

bool initialize_debug_layer(void)
{
    bool status = true;

    VkDebugReportCallbackCreateInfoEXT callback_info;
    callback_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    callback_info.pNext = NULL;
    callback_info.flags = VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT;
    callback_info.pfnCallback = debug_callback;
    callback_info.pUserData = NULL;

    if(g_vk_ctx.register_debug_callback(g_vk_ctx.instance, &callback_info, g_vk_ctx.allocation_callbacks, &g_vk_ctx.debug_callback) != VK_SUCCESS)
    {
        printf("Failed to register debug callback\n");
        status = false;
    }

    return status;
}

bool enumerate_gpus(uint32_t* gpu_count, struct gpu_info** gpu_info_array)
{
    bool status = true;

    uint32_t num_physical_devices = 0;

    VkPhysicalDevice* physical_device_handles = NULL;

    struct gpu_info* info_array = NULL;

    if(g_vk_ctx.enumerate_physical_devices(g_vk_ctx.instance, &num_physical_devices, NULL) != VK_SUCCESS)
    {
        status = false;
        printf("Failed to get number of devices\n");
    }

    if(status)
    {
        physical_device_handles = malloc(num_physical_devices * sizeof(VkPhysicalDevice));

        if(physical_device_handles == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(g_vk_ctx.enumerate_physical_devices(g_vk_ctx.instance, &num_physical_devices, physical_device_handles) != VK_SUCCESS)
        {
            status = false;
            printf("Failed to get devices\n");
        }
    }

    if(status)
    {
        info_array = malloc(num_physical_devices * sizeof(struct gpu_info));

        if(info_array == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 0; status && (i < num_physical_devices); i++)
        {
            info_array[i].handle = physical_device_handles[i];
            g_vk_ctx.get_physical_device_properties(physical_device_handles[i], &info_array[i].properties);
            g_vk_ctx.get_physical_device_features(physical_device_handles[i], &info_array[i].features);
            
            status = get_gpu_queue_info(physical_device_handles[i], &info_array[i].queue_group_count, &info_array[i].queue_group_properties);
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < num_physical_devices; i++)
        {
            print_gpu_info(i, &info_array[i]);
        }
    }

    if (status)
    {
        *gpu_count = num_physical_devices;
        *gpu_info_array = info_array;
    }
    else
    {
        free_gpu_info(num_physical_devices, info_array);
        info_array = NULL;
    }

    if(physical_device_handles != NULL)
    {
        free(physical_device_handles);
        physical_device_handles = NULL;
    }

    return status;
}

void free_gpu_info(uint32_t gpu_count, struct gpu_info* gpu_info_array)
{
    if(gpu_info_array != NULL)
    {
        for(uint32_t i = 0; i < gpu_count; i++)
        {
            if(gpu_info_array[i].queue_group_properties != NULL)
            {
                free(gpu_info_array[i].queue_group_properties);
                gpu_info_array[i].queue_group_properties = NULL;
            }
        }

        free(gpu_info_array);
        gpu_info_array = NULL;
    }
}

bool initialize_device(void)
{
    bool status = true;

    uint32_t gpu_count = 0;
    uint32_t gpu_index = 0;
    struct gpu_info* gpu_info = NULL;

    struct layer_list layers = { 0 };

    uint32_t num_extensions = 0;
    const char* extensions[MAX_EXTENSIONS] = { 0 };

    if(status)
    {
        status = enumerate_gpus(&gpu_count, &gpu_info);
    }

    if(status)
    {
        while(gpu_index < gpu_count)
        {
            if(gpu_info[gpu_index].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                printf("Use device %u\n", gpu_index);
                break;
            }

            gpu_index++;
        }

        if(gpu_index < gpu_count)
        {
            g_vk_ctx.physical_device = gpu_info[gpu_index].handle;
        }
        else
        {
            status = false;
            printf("Could not find discrete gpu\n");
        }
    }

    if(status)
    {
        status = enumerate_device_layers_and_extensions(g_vk_ctx.physical_device, &layers);
    }

    if(status)
    {
        status = add_extension(&layers.extension_lists[0], extensions, &num_extensions, "VK_KHR_swapchain");
    }

    if(status)
    {
        uint32_t queue_group_index = 0;

        while(queue_group_index < gpu_info[gpu_index].queue_group_count)
        {
            if(gpu_info[gpu_index].queue_group_properties[queue_group_index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                printf("Use queue group %u\n", queue_group_index);
                break;
            }

            queue_group_index++;
        }

        if(queue_group_index < gpu_info[gpu_index].queue_group_count)
        {
            g_vk_ctx.graphics_queue_family = queue_group_index;
        }
        else
        {
            status = false;
            printf("Could not find queue group\n");
        }
    }

    if(status)
    {
        const float queue_priorities[VK_CTX_NUM_GRAPHICS_QUEUES] = { 1.0f };

        uint32_t queue_count = gpu_info[gpu_index].queue_group_properties[g_vk_ctx.graphics_queue_family].queueCount;

        if(queue_count > VK_CTX_NUM_GRAPHICS_QUEUES)
        {
            queue_count = VK_CTX_NUM_GRAPHICS_QUEUES;
        }

        VkDeviceQueueCreateInfo queue_info;
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.pNext = NULL;
        queue_info.flags = 0;
        queue_info.queueFamilyIndex = g_vk_ctx.graphics_queue_family;
        queue_info.queueCount = queue_count;
        queue_info.pQueuePriorities = queue_priorities;

        VkDeviceCreateInfo device_info;
        device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_info.pNext = NULL;
        device_info.flags = 0;
        device_info.queueCreateInfoCount = 1;
        device_info.pQueueCreateInfos = &queue_info;
        device_info.enabledLayerCount = 0;
        device_info.ppEnabledLayerNames = NULL;
        device_info.enabledExtensionCount = num_extensions;
        device_info.ppEnabledExtensionNames = extensions;
        device_info.pEnabledFeatures = NULL;

        if(g_vk_ctx.create_device(g_vk_ctx.physical_device, &device_info, g_vk_ctx.allocation_callbacks, &g_vk_ctx.device) != VK_SUCCESS)
        {
            status = false;
            printf("Failed to create vulkan device\n");
        }
    }

    if(status)
    {
        status = initialize_device_function_pointers();
    }

    free_layers(&layers);

    free_gpu_info(gpu_count, gpu_info);
    gpu_info = NULL;

    return status;
}

void print_gpu_info(uint32_t gpu_index, struct gpu_info* gpu_info)
{
    printf("Device %u: %s\n", gpu_index, gpu_info->properties.deviceName);

    const char* type = "Unknown";
    switch(gpu_info->properties.deviceType )
    {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            type = "Integrated GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            type = "Discrete GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            type = "Virtual GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            type = "CPU";
            break;
        default:
            break;
    }
    printf("\tType: %s\n", type);

    printf("\tVendor: 0x%X\n", gpu_info->properties.vendorID);
    printf("\tDevice ID: 0x%X\n", gpu_info->properties.deviceID);
    printf("\tDriver Version: 0x%X\n", gpu_info->properties.driverVersion);
    printf("\tAPI Version: 0x%X\n", gpu_info->properties.apiVersion);

    printf("\tFeatures:\n");
    if(gpu_info->features.robustBufferAccess) { printf("\t\tRobust buffer access\n"); }
    if(gpu_info->features.fullDrawIndexUint32) { printf("\t\tFull draw index\n"); }
    if(gpu_info->features.imageCubeArray) { printf("\t\tImage cube array\n"); }
    if(gpu_info->features.independentBlend) { printf("\t\tIndependent blend\n"); }
    if(gpu_info->features.geometryShader) { printf("\t\tGeometry shader\n"); }
    if(gpu_info->features.tessellationShader) { printf("\t\tTessellation shader\n"); }
    if(gpu_info->features.sampleRateShading) { printf("\t\tSample rate shading\n"); }
    if(gpu_info->features.dualSrcBlend) { printf("\t\tDual source blend\n"); }
    if(gpu_info->features.logicOp) { printf("\t\tLogic op\n"); }
    if(gpu_info->features.multiDrawIndirect) { printf("\t\tMulti draw indirect\n"); }
    if(gpu_info->features.drawIndirectFirstInstance) { printf("\t\tDraw indirect first instance\n"); }
    if(gpu_info->features.depthClamp) { printf("\t\tDepth clamp\n"); }
    if(gpu_info->features.depthBiasClamp) { printf("\t\tDepth bias clamp\n"); }
    if(gpu_info->features.fillModeNonSolid) { printf("\t\tFill mode non solid\n"); }
    if(gpu_info->features.depthBounds) { printf("\t\tDepth bounds\n"); }
    if(gpu_info->features.wideLines) { printf("\t\tWide lines\n"); }
    if(gpu_info->features.largePoints) { printf("\t\tLarge points\n"); }
    if(gpu_info->features.alphaToOne) { printf("\t\tAlpha to one\n"); }
    if(gpu_info->features.multiViewport) { printf("\t\tMulti viewport\n"); }
    if(gpu_info->features.samplerAnisotropy) { printf("\t\tSampler anisotropy\n"); }
    if(gpu_info->features.textureCompressionETC2) { printf("\t\tTexture compression etc2\n"); }
    if(gpu_info->features.textureCompressionASTC_LDR) { printf("\t\tTexture compression astc ldr\n"); }
    if(gpu_info->features.textureCompressionBC) { printf("\t\tTexture compression bc\n"); }
    if(gpu_info->features.occlusionQueryPrecise) { printf("\t\tOcclusion query precise\n"); }
    if(gpu_info->features.pipelineStatisticsQuery) { printf("\t\tPipeline statistics query\n"); }
    if(gpu_info->features.vertexPipelineStoresAndAtomics) { printf("\t\tVertex pipeline stores and atomics\n"); }
    if(gpu_info->features.fragmentStoresAndAtomics) { printf("\t\tFragment stores and atomics\n"); }
    if(gpu_info->features.shaderTessellationAndGeometryPointSize) { printf("\t\tShader tessellation and geometry point size\n"); }
    if(gpu_info->features.shaderImageGatherExtended) { printf("\t\tShader image gather extended\n"); }
    if(gpu_info->features.shaderStorageImageExtendedFormats) { printf("\t\tShader storage image extended formats\n"); }
    if(gpu_info->features.shaderStorageImageMultisample) { printf("\t\tShader storage image multisample\n"); }
    if(gpu_info->features.shaderStorageImageReadWithoutFormat) { printf("\t\tShader storage image read without format\n"); }
    if(gpu_info->features.shaderStorageImageWriteWithoutFormat) { printf("\t\tShader storage image write without format\n"); }
    if(gpu_info->features.shaderUniformBufferArrayDynamicIndexing) { printf("\t\tShader uniform buffer array dynamic indexing\n"); }
    if(gpu_info->features.shaderSampledImageArrayDynamicIndexing) { printf("\t\tShader sampled image array dynamic indexing\n"); }
    if(gpu_info->features.shaderStorageBufferArrayDynamicIndexing) { printf("\t\tShader storage buffer array dynamic indexing\n"); }
    if(gpu_info->features.shaderStorageImageArrayDynamicIndexing) { printf("\t\tShader storage image array dynamic indexing\n"); }
    if(gpu_info->features.shaderClipDistance) { printf("\t\tShader clip distance\n"); }
    if(gpu_info->features.shaderCullDistance) { printf("\t\tShader cull distance\n"); }
    if(gpu_info->features.shaderFloat64) { printf("\t\tShader float 64\n"); }
    if(gpu_info->features.shaderInt64) { printf("\t\tShader int 64\n"); }
    if(gpu_info->features.shaderInt16) { printf("\t\tShader int 16\n"); }
    if(gpu_info->features.shaderResourceResidency) { printf("\t\tShader resource residency\n"); }
    if(gpu_info->features.shaderResourceMinLod) { printf("\t\tShader resource min lod\n"); }
    if(gpu_info->features.sparseBinding) { printf("\t\tSparse binding\n"); }
    if(gpu_info->features.sparseResidencyBuffer) { printf("\t\tSparse residency buffer\n"); }
    if(gpu_info->features.sparseResidencyImage2D) { printf("\t\tSparse residency image 2D\n"); }
    if(gpu_info->features.sparseResidencyImage3D) { printf("\t\tSparse residency image 3D\n"); }
    if(gpu_info->features.sparseResidency2Samples) { printf("\t\tSparse residency 2 samples\n"); }
    if(gpu_info->features.sparseResidency4Samples) { printf("\t\tSparse residency 4 samples\n"); }
    if(gpu_info->features.sparseResidency8Samples) { printf("\t\tSparse residency 8 samples\n"); }
    if(gpu_info->features.sparseResidency16Samples) { printf("\t\tSparse residency 16 samples\n"); }
    if(gpu_info->features.sparseResidencyAliased) { printf("\t\tSparse residency aliased\n"); }
    if(gpu_info->features.variableMultisampleRate) { printf("\t\tVariable multi-sample rate\n"); }
    if(gpu_info->features.inheritedQueries) { printf("\t\tInherited queries\n"); }

    for(uint32_t i = 0; i < gpu_info->queue_group_count; i++)
    {
        printf("\tQueue group %u:\n", i);
        printf("\t\tQueue count: %u\n", gpu_info->queue_group_properties[i].queueCount);
        
        if(gpu_info->queue_group_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) { printf("\t\tGraphics supported\n"); }
        if(gpu_info->queue_group_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) { printf("\t\tCompute supported\n"); }
        if(gpu_info->queue_group_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) { printf("\t\tTransfer supported\n"); }
        if(gpu_info->queue_group_properties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) { printf("\t\tSparse binding supported\n"); }
        if(gpu_info->queue_group_properties[i].queueFlags & VK_QUEUE_PROTECTED_BIT) { printf("\t\tProtected memory supported\n"); }
        if(gpu_info->queue_group_properties[i].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) { printf("\t\tVideo decode supported\n"); }
        if(gpu_info->queue_group_properties[i].queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) { printf("\t\tVideo encode supported\n"); }
        if(gpu_info->queue_group_properties[i].queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) { printf("\t\tOptical flow supported\n"); }
    }
}

bool get_gpu_queue_info(VkPhysicalDevice physical_device, uint32_t* num_queue_groups, struct VkQueueFamilyProperties** queue_group_properties)
{
    bool status = true;

    uint32_t num_groups = 0;
    VkQueueFamilyProperties* groups = NULL;

    g_vk_ctx.get_physical_queue_group_properties(physical_device, &num_groups, NULL);

    if(num_groups > 0)
    {
        groups = malloc(num_groups * sizeof(VkQueueFamilyProperties));

        if(groups == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(num_groups > 0)
        {
            g_vk_ctx.get_physical_queue_group_properties(physical_device, &num_groups, groups);
        }

        *num_queue_groups = num_groups;
        *queue_group_properties = groups;
    }

    return status;
}

bool enumerate_device_layers_and_extensions(VkPhysicalDevice physical_device, struct layer_list* device_layers)
{
    bool status = true;

    uint32_t num_layers = 0;
    struct layer_list layers = { 0 };

    if(g_vk_ctx.enumerate_device_layers(physical_device, &num_layers, NULL) == VK_SUCCESS)
    {
        layers.count = num_layers + 1; // +1 for vulkan implementation
    }
    else
    {
        status = false;
        printf("Failed to get number of layers\n");
    }

    if(status)
    {
        layers.array = calloc(layers.count, sizeof(VkLayerProperties));

        if (layers.array == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        layers.extension_lists = calloc(layers.count, sizeof(struct extension_list));

        if(layers.extension_lists == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        // enumerate the extensions provided by the vulkan implementation
        printf("Device layer: Vulkan Implementation\n");
        status = enumerate_device_extensions(physical_device, NULL, &layers.extension_lists[0]);
    }

    if(status)
    {
        if(g_vk_ctx.enumerate_device_layers(physical_device, &num_layers, &layers.array[1]) != VK_SUCCESS)
        {
            status = false;
            printf("Failed to get layers\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 1; i <= num_layers; i++)
        {
            printf("Device layer %s (0x%X, %u): %s\n", layers.array[i].layerName, layers.array[i].specVersion, layers.array[i].implementationVersion, layers.array[i].description);
            status = enumerate_device_extensions(physical_device, layers.array[i].layerName, &layers.extension_lists[i]);
        }
    }

    if(status)
    {
        device_layers->array = layers.array;
        device_layers->count = layers.count;
        device_layers->extension_lists = layers.extension_lists;
    }
    else
    {
        free_layers(&layers);
    }

    return status;
}

bool enumerate_device_extensions(VkPhysicalDevice physical_device, const char* layer, struct extension_list* device_extensions)
{
    bool status = true;

    struct extension_list extensions = { 0 };

    if(g_vk_ctx.enumerate_device_extensions(physical_device, layer, &extensions.count, NULL) != VK_SUCCESS)
    {
        status = false;
        printf("Failed to get number of extensions\n");
    }

    if(status)
    {
        extensions.array = calloc(extensions.count, sizeof(VkExtensionProperties));

        if(extensions.array == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(g_vk_ctx.enumerate_device_extensions(physical_device, layer, &extensions.count, extensions.array) != VK_SUCCESS)
        {
            status = false;
            printf("Failed to get extensions\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < extensions.count; i++)
        {
            printf("\tDevice extension: %s\n", extensions.array[i].extensionName);
        }
    }

    if(status)
    {
        device_extensions->array = extensions.array;
        device_extensions->count = extensions.count;
    }
    else
    {
        free_extensions(&extensions);
    }

    return status;
}
