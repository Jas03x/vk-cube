
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "vk_context.h"

struct vk_context vk_ctx = { 0 };

enum { INVALID_INDEX = 0xFFFFFFFF };

struct gpu_info
{
    vk_physical_device                   handle;
    struct vk_physical_device_properties properties;
    struct vk_physical_device_features   features;

    uint32_t                             queue_group_count;
    struct vk_queue_group_properties*    p_queue_group_properties;
};

struct extension_list
{
    uint32_t               count;
    struct vk_extension*   array;
};

struct layer_list
{
    uint32_t               count;
    struct vk_layer*       array;

    struct extension_list* extension_lists;
};

bool initialize_global_function_pointers(void);
bool initialize_instance_function_pointers(void);
bool initialize_physical_device_function_pointers(void);

bool initialize_instance(void);
bool initialize_device(void);
bool initialize_debug_layer(void);

bool enumerate_instance_layers(struct layer_list* p_layers);
bool enumerate_instance_extensions(const char* p_layer, struct extension_list* p_extensions);
bool enumerate_gpus(uint32_t* p_gpu_count, struct gpu_info** p_gpu_info_array);
bool enumerate_device_layers_and_extensions(vk_physical_device h_physical_device);
bool enumerate_device_extensions(vk_physical_device h_physical_device, const char* p_layer);

bool get_gpu_queue_info(vk_physical_device h_physical_device, uint32_t* p_num_queue_groups, struct vk_queue_group_properties** p_queue_group_properties);

void print_gpu_info(uint32_t gpu_index, struct gpu_info* p_gpu_info);

uint32_t find_extension(struct layer_list* layers, const char* layer_name, const char* extension_name);

void free_layers(struct layer_list* layers);
void free_extensions(struct extension_list* extensions);
void free_gpu_info(uint32_t gpu_count, struct gpu_info* p_gpu_info_array);

uint32_t debug_callback(uint32_t flags, uint32_t object_type, uint64_t object, uint32_t location, int32_t message_code, const char* layer_prefix, const char* message, void* user_data)
{
    enum { buffer_size = 256 };

    uint32_t index = 0;
    char buffer[buffer_size] = { 0 };

    if(flags & vk_debug_callback_information_bit)
    {
        buffer[index++] = 'i';
    }

    if(flags & vk_debug_callback_warning_bit)
    {
        buffer[index++] = 'w';
    }

    if(flags & vk_debug_callback_performance_warning_bit)
    {
        buffer[index++] = 'p';
    }

    if(flags & vk_debug_callback_error_bit)
    {
        buffer[index++] = 'E';
    }

    if(flags & vk_debug_callback_debug_bit)
    {
        buffer[index++] = 'D';
    }

    buffer[index++] = ':';

    switch(object_type)
    {
        case vk_debug_callback_object_type__instance:
            index += snprintf(&buffer[index], buffer_size - index, "instance");
            break;
        case vk_debug_callback_object_type__physical_device:
            index += snprintf(&buffer[index], buffer_size - index, "physical_device");
            break;
        case vk_debug_callback_object_type__device:
            index += snprintf(&buffer[index], buffer_size - index, "device");
            break;
        case vk_debug_callback_object_type__queue:
            index += snprintf(&buffer[index], buffer_size - index, "queue");
            break;
        case vk_debug_callback_object_type__semaphore:
            index += snprintf(&buffer[index], buffer_size - index, "semaphore");
            break;
        case vk_debug_callback_object_type__command_buffer:
            index += snprintf(&buffer[index], buffer_size - index, "command_buffer");
            break;
        case vk_debug_callback_object_type__fence:
            index += snprintf(&buffer[index], buffer_size - index, "fence");
            break;
        case vk_debug_callback_object_type__device_memory:
            index += snprintf(&buffer[index], buffer_size - index, "device_memory");
            break;
        case vk_debug_callback_object_type__buffer:
            index += snprintf(&buffer[index], buffer_size - index, "buffer");
            break;
        case vk_debug_callback_object_type__image:
            index += snprintf(&buffer[index], buffer_size - index, "image");
            break;
        case vk_debug_callback_object_type__event:
            index += snprintf(&buffer[index], buffer_size - index, "event");
            break;
        case vk_debug_callback_object_type__query_pool:
            index += snprintf(&buffer[index], buffer_size - index, "query_pool");
            break;
        case vk_debug_callback_object_type__buffer_view:
            index += snprintf(&buffer[index], buffer_size - index, "buffer_view");
            break;
        case vk_debug_callback_object_type__image_view:
            index += snprintf(&buffer[index], buffer_size - index, "image_view");
            break;
        case vk_debug_callback_object_type__shader_module:
            index += snprintf(&buffer[index], buffer_size - index, "shader_module");
            break;
        case vk_debug_callback_object_type__pipeline_cache:
            index += snprintf(&buffer[index], buffer_size - index, "pipeline_cache");
            break;
        case vk_debug_callback_object_type__pipeline_layout:
            index += snprintf(&buffer[index], buffer_size - index, "pipeline_layout");
            break;
        case vk_debug_callback_object_type__render_pass:
            index += snprintf(&buffer[index], buffer_size - index, "render_pass");
            break;
        case vk_debug_callback_object_type__pipeline:
            index += snprintf(&buffer[index], buffer_size - index, "pipeline");
            break;
        case vk_debug_callback_object_type__descriptor_set_layout:
            index += snprintf(&buffer[index], buffer_size - index, "descriptor_set_layout");
            break;
        case vk_debug_callback_object_type__sampler:
            index += snprintf(&buffer[index], buffer_size - index, "sampler");
            break;
        case vk_debug_callback_object_type__descriptor_pool:
            index += snprintf(&buffer[index], buffer_size - index, "descriptor_pool");
            break;
        case vk_debug_callback_object_type__descriptor_set:
            index += snprintf(&buffer[index], buffer_size - index, "descriptor_set");
            break;
        case vk_debug_callback_object_type__framebuffer:
            index += snprintf(&buffer[index], buffer_size - index, "framebuffer");
            break;
        case vk_debug_callback_object_type__command_pool:
            index += snprintf(&buffer[index], buffer_size - index, "command_pool");
            break;
        case vk_debug_callback_object_type__surface:
            index += snprintf(&buffer[index], buffer_size - index, "surface");
            break;
        case vk_debug_callback_object_type__swapchain:
            index += snprintf(&buffer[index], buffer_size - index, "swapchain");
            break;
        case vk_debug_callback_object_type__debug_report:
            index += snprintf(&buffer[index], buffer_size - index, "debug_report");
            break;
        case vk_debug_callback_object_type__display:
            index += snprintf(&buffer[index], buffer_size - index, "display");
            break;
        case vk_debug_callback_object_type__display_mode:
            index += snprintf(&buffer[index], buffer_size - index, "display_mode");
            break;
        case vk_debug_callback_object_type__validation_cache:
            index += snprintf(&buffer[index], buffer_size - index, "validation_cache");
            break;
        case vk_debug_callback_object_type__sampler_ycbcr_conversion:
            index += snprintf(&buffer[index], buffer_size - index, "conversion");
            break;
        case vk_debug_callback_object_type__descriptor_update_template:
            index += snprintf(&buffer[index], buffer_size - index, "descriptor_update_template");
            break;
        default:
            index += snprintf(&buffer[index], buffer_size - index, "0x%X", object_type);
            break;
    }

    index += snprintf(&buffer[index], buffer_size - index, " 0x%llX ", object);
    index += snprintf(&buffer[index], buffer_size - index, " 0x%X ", location);
    index += snprintf(&buffer[index], buffer_size - index, " 0x%X ", message_code);
    index += snprintf(&buffer[index], buffer_size - index, " %s ", layer_prefix);
    index += snprintf(&buffer[index], buffer_size - index, " %s ", message);
    index += snprintf(&buffer[index], buffer_size - index, " 0x%llX ", (uint64_t) user_data);

    printf("%s\n", buffer);

    return vk_true;
}

bool initialize_vulkan_context(pfn_vk_get_instance_proc_addr pfn_get_instance_proc_addr)
{
    bool status = true;

    vk_ctx.get_instance_proc_addr = pfn_get_instance_proc_addr;

    if(status)
    {
        status = initialize_global_function_pointers();
    }

    if(status)
    {
        status = initialize_instance();
    }

    if(status)
    {
        status = initialize_device();
    }

    return status;
}

void uninitialize_vulkan_context(void)
{
    vk_ctx.wait_for_device_idle(vk_ctx.h_device);
    //vk_ctx.destroy_device(vk_ctx.h_device, NULL);
    //vk_ctx.h_device = NULL;

    if(vk_ctx.h_debug_callback != NULL)
    {
        vk_ctx.unregister_debug_callback(vk_ctx.h_instance, vk_ctx.h_debug_callback, NULL);
    }

    vk_ctx.destroy_instance(vk_ctx.h_instance, NULL);
    vk_ctx.h_instance = NULL;
}

bool load_function_pointer(vk_instance h_instance, const char* p_name, void** p_pfn)
{
    bool status = true;

    if(p_pfn != NULL)
    {
        (*p_pfn) = vk_ctx.get_instance_proc_addr(h_instance, p_name);

        if((*p_pfn) == NULL)
        {
            status = false;
            printf("error: failed to load function pointer %s\n", p_name);
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

    status &= load_function_pointer(NULL, "vkCreateInstance", (void**) &vk_ctx.create_instance);
    status &= load_function_pointer(NULL, "vkEnumerateInstanceVersion", (void**) &vk_ctx.get_version);
    status &= load_function_pointer(NULL, "vkEnumerateInstanceLayerProperties", (void**) &vk_ctx.enumerate_layers);
    status &= load_function_pointer(NULL, "vkEnumerateInstanceExtensionProperties", (void**) &vk_ctx.enumerate_extensions);

    return status;
}

bool initialize_instance_function_pointers(void)
{
    bool status = true;

    status &= load_function_pointer(vk_ctx.h_instance, "vkDestroyInstance", (void**) &vk_ctx.destroy_instance);
    status &= load_function_pointer(vk_ctx.h_instance, "vkEnumeratePhysicalDevices", (void**) &vk_ctx.enumerate_devices);
    status &= load_function_pointer(vk_ctx.h_instance, "vkGetPhysicalDeviceProperties", (void**) &vk_ctx.get_physical_device_properties);
    status &= load_function_pointer(vk_ctx.h_instance, "vkGetPhysicalDeviceFeatures", (void**) &vk_ctx.get_physical_device_features);
    status &= load_function_pointer(vk_ctx.h_instance, "vkGetPhysicalDeviceQueueFamilyProperties", (void**) &vk_ctx.get_physical_queue_group_properties);
    status &= load_function_pointer(vk_ctx.h_instance, "vkCreateDevice", (void**) &vk_ctx.create_device);
    status &= load_function_pointer(vk_ctx.h_instance, "vkDestroyDevice", (void**) &vk_ctx.destroy_device);
    status &= load_function_pointer(vk_ctx.h_instance, "vkDeviceWaitIdle", (void**) &vk_ctx.wait_for_device_idle);
    status &= load_function_pointer(vk_ctx.h_instance, "vkCreateDebugReportCallbackEXT", (void**) &vk_ctx.register_debug_callback);
    status &= load_function_pointer(vk_ctx.h_instance, "vkDestroyDebugReportCallbackEXT", (void*) &vk_ctx.unregister_debug_callback);

    return status;
}

bool initialize_physical_device_function_pointers(void)
{
    bool status = true;

    status &= load_function_pointer(vk_ctx.h_instance, "vkEnumerateDeviceLayerProperties", (void**) &vk_ctx.enumerate_device_layers);
    status &= load_function_pointer(vk_ctx.h_instance, "vkEnumerateDeviceExtensionProperties", (void**) &vk_ctx.enumerate_device_extensions);

    return status;
}

bool initialize_logical_device_function_pointers(void)
{
    bool status = true;

    status &= load_function_pointer(vk_ctx.h_instance, "vkGetDeviceProcAddr", (void**) &vk_ctx.get_device_proc_addr);

    return status;
}

bool enumerate_instance_layers(struct layer_list* p_layers)
{
    bool status = true;

    uint32_t num_layers = 0;
    struct layer_list layers = { 0 };

    if(vk_ctx.enumerate_layers(&num_layers, NULL) == vk_success)
    {
        layers.count = num_layers + 1; // +1 for vulkan implementation
    }
    else
    {
        status = false;
        printf("failed to get number of layers\n");
    }

    if(status)
    {
        layers.array = calloc(layers.count, sizeof(struct vk_layer));

        if(layers.array == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        layers.extension_lists = calloc(layers.count, sizeof(struct extension_list));

        if(layers.extension_lists == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(vk_ctx.enumerate_layers(&num_layers, &layers.array[1]) != vk_success) // start reading at element 1 because element 0 is the vulkan implementation
        {
            status = false;
            printf("failed to get layers\n");
        }
    }

    if(status)
    {
        // enumerate the extensions provided by the vulkan implementation
        printf("Layer: Vulkan implementation\n");
        status = enumerate_instance_extensions(NULL, &layers.extension_lists[0]);
    }

    if(status)
    {
        for(uint32_t i = 1; i <= num_layers; i++) // start at 1 because index 0 used for vulkan implementation
        {
            printf("Layer %s (0x%X, %u): %s\n", layers.array[i].name, layers.array[i].spec_version, layers.array[i].impl_version, layers.array[i].desc);
            status = enumerate_instance_extensions(layers.array[i].name, &layers.extension_lists[i]);
        }
    }

    if(status)
    {
        p_layers->array = layers.array;
        p_layers->count = layers.count;
        p_layers->extension_lists = layers.extension_lists;
    }
    else
    {
        free_layers(&layers);
    }

    return status;
}

uint32_t find_extension(struct layer_list* layers, const char* layer_name, const char* extension_name)
{
    uint32_t index = INVALID_INDEX;

    for(uint32_t i = 0; i < layers->count; i++)
    {
        if(strncmp(layers->array[i].name, layer_name, VK_MAX_NAME_LENGTH) == 0)
        {
            for(uint32_t j = 0; j < layers->extension_lists[i].count; j++)
            {
                if(strncmp(layers->extension_lists[i].array[j].name, extension_name, VK_MAX_NAME_LENGTH) == 0)
                {
                    index = j;
                    break;
                }
            }

            break;
        }
    }

    return index;
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

bool enumerate_instance_extensions(const char* p_layer, struct extension_list* p_extensions)
{
    bool status = true;

    struct extension_list extensions = { 0 };

    if(vk_ctx.enumerate_extensions(p_layer, &extensions.count, NULL) != vk_success)
    {
        status = false;
        printf("failed to get number of extensions\n");
    }

    if(status)
    {
        extensions.array = calloc(extensions.count, sizeof(struct vk_extension));

        if(extensions.array == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(vk_ctx.enumerate_extensions(p_layer, &extensions.count, extensions.array) != vk_success)
        {
            status = false;
            printf("failed to get extensions\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < extensions.count; i++)
        {
            printf("\tExtension: %s\n", extensions.array[i].name);
        }
    }

    if(status)
    {
        p_extensions->array = extensions.array;
        p_extensions->count = extensions.count;
    }
    else
    {
        free_extensions(&extensions);
    }

    return status;
}

bool initialize_instance(void)
{
    bool status = true;

    enum { max_extensions = 16 };

    struct layer_list layers = { 0 };

    uint32_t num_extensions = 0;
    uint32_t index = INVALID_INDEX;

    const char* extensions[max_extensions] = { 0 };    

    if(status)
    {
        status = enumerate_instance_layers(&layers);
    }

#ifdef DEBUG
    index = find_extension(&layers, "", "VK_EXT_debug_report");
    if(index != INVALID_INDEX)
    {
        extensions[num_extensions++] = layers.extension_lists[0].array[index].name;
    }
#endif

    if(status)
    {
        struct vk_application_info app_info = { 0 };
        app_info.s_type = vk_application_info;
        app_info.p_next = NULL;
        app_info.p_app_name = "vk-cube";
        app_info.app_version = 1;
        app_info.p_engine_name = "vk-cube";
        app_info.engine_version = 1;
        app_info.api_version = VK_VERSION(0, 1, 0, 0);

        struct vk_instance_info instance_info = { 0 };
        instance_info.s_type = vk_instance_info;
        instance_info.p_next = NULL;
        instance_info.p_app_info = &app_info;
        instance_info.layer_count = 0;
        instance_info.p_layer_names = NULL;
        instance_info.extension_count = num_extensions;
        instance_info.p_extension_names = extensions;

        if(vk_ctx.create_instance(&instance_info, NULL, &vk_ctx.h_instance) != vk_success)
        {
            status = false;
            printf("failed to create vulkan instance\n");
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

    struct vk_debug_callback_info callback_info;
    callback_info.s_type = vk_structure_type_debug_report_callback_info;
    callback_info.p_next = NULL;
    callback_info.flags = vk_debug_callback_all_bits;
    callback_info.pfn_callback = debug_callback;
    callback_info.user_data = NULL;

    if(vk_ctx.register_debug_callback(vk_ctx.h_instance, &callback_info, NULL, &vk_ctx.h_debug_callback) != vk_success)
    {
        printf("error: failed to register debug callback\n");
        status = false;
    }

    return status;
}

bool enumerate_gpus(uint32_t* p_gpu_count, struct gpu_info** p_gpu_info_array)
{
    bool status = true;

    uint32_t num_physical_devices = 0;

    vk_physical_device* p_physical_device_handles = NULL;

    struct gpu_info* p_info_array = NULL;

    if(vk_ctx.enumerate_devices(vk_ctx.h_instance, &num_physical_devices, NULL) != vk_success)
    {
        status = false;
        printf("failed to get number of devices\n");
    }

    if(status)
    {
        p_physical_device_handles = malloc(num_physical_devices * sizeof(vk_physical_device));

        if(p_physical_device_handles == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(vk_ctx.enumerate_devices(vk_ctx.h_instance, &num_physical_devices, p_physical_device_handles) != vk_success)
        {
            status = false;
            printf("failed to get devices\n");
        }
    }

    if(status)
    {
        p_info_array = malloc(num_physical_devices * sizeof(struct gpu_info));

        if(p_info_array == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 0; status && (i < num_physical_devices); i++)
        {
            p_info_array[i].handle = p_physical_device_handles[i];
            vk_ctx.get_physical_device_properties(p_physical_device_handles[i], &p_info_array[i].properties);
            vk_ctx.get_physical_device_features(p_physical_device_handles[i], &p_info_array[i].features);
            
            status = get_gpu_queue_info(p_physical_device_handles[i], &p_info_array[i].queue_group_count, &p_info_array[i].p_queue_group_properties);
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < num_physical_devices; i++)
        {
            print_gpu_info(i, &p_info_array[i]);
        }
    }

    if (status)
    {
        *p_gpu_count = num_physical_devices;
        *p_gpu_info_array = p_info_array;
    }
    else
    {
        free_gpu_info(num_physical_devices, p_info_array);
        p_info_array = NULL;
    }

    if(p_physical_device_handles != NULL)
    {
        free(p_physical_device_handles);
        p_physical_device_handles = NULL;
    }

    return status;
}

void free_gpu_info(uint32_t gpu_count, struct gpu_info* p_gpu_info_array)
{
    if(p_gpu_info_array != NULL)
    {
        for(uint32_t i = 0; i < gpu_count; i++)
        {
            if(p_gpu_info_array[i].p_queue_group_properties != NULL)
            {
                free(p_gpu_info_array[i].p_queue_group_properties);
                p_gpu_info_array[i].p_queue_group_properties = NULL;
            }
        }

        free(p_gpu_info_array);
        p_gpu_info_array = NULL;
    }
}

bool initialize_device(void)
{
    bool status = true;

    uint32_t gpu_count = 0;
    uint32_t gpu_index = 0;
    uint32_t queue_group_index = 0;
    struct gpu_info* p_gpu_info = NULL;

    if(status)
    {
        status = enumerate_gpus(&gpu_count, &p_gpu_info);
    }

    if(status)
    {
        while(gpu_index < gpu_count)
        {
            if(p_gpu_info[gpu_index].properties.device_type == vk_device_type__discrete_gpu)
            {
                printf("use device %u\n", gpu_index);
                break;
            }

            gpu_index++;
        }

        if(gpu_index >= gpu_count)
        {
            status = false;
            printf("error: could not find discrete gpu\n");
        }
    }

    if(status)
    {
        status = initialize_physical_device_function_pointers();
    }

    if(status)
    {
        status = enumerate_device_layers_and_extensions(p_gpu_info[gpu_index].handle);
    }

    if(status)
    {
        while(queue_group_index < p_gpu_info[gpu_index].queue_group_count)
        {
            if(p_gpu_info[gpu_index].p_queue_group_properties[queue_group_index].queue_flags.graphics)
            {
                printf("use queue group %u\n", queue_group_index);
                break;
            }

            queue_group_index++;
        }

        if(queue_group_index >= p_gpu_info[gpu_index].queue_group_count)
        {
            status = false;
            printf("error: could not find queue group\n");
        }
    }

    if(status)
    {
        enum { MAX_QUEUES = 2 };

        const float p_queue_priorities[MAX_QUEUES] = { 1.0f, 1.0f };

        uint32_t queue_count = p_gpu_info[gpu_index].p_queue_group_properties[queue_group_index].queue_count;

        if(queue_count > MAX_QUEUES)
        {
            queue_count = MAX_QUEUES;
        }

        struct vk_queue_creation_info queue_info = { 0 };
        queue_info.s_type = vk_queue_create_info;
        queue_info.p_next = NULL;
        queue_info.queue_group_index = queue_group_index;
        queue_info.queue_count = queue_count;
        queue_info.p_queue_priorities = p_queue_priorities;

        struct vk_device_creation_info device_info = { 0 };
        device_info.s_type = vk_device_create_info;
        device_info.p_next = NULL;
        device_info.queue_info_count = 1;
        device_info.p_queue_info_array = &queue_info;
        device_info.layer_count = 0;
        device_info.p_layers = NULL;
        device_info.extension_count = 0;
        device_info.p_extensions = NULL;
        device_info.features = NULL;

        if(vk_ctx.create_device(p_gpu_info[gpu_index].handle, &device_info, NULL, &vk_ctx.h_device) != vk_success)
        {
            status = false;
            printf("failed to create vulkan device\n");
        }
    }

    if(status)
    {
        status = initialize_logical_device_function_pointers();
    }

    free_gpu_info(gpu_count, p_gpu_info);
    p_gpu_info = NULL;

    return status;
}


void print_gpu_info(uint32_t gpu_index, struct gpu_info* p_gpu_info)
{
    printf("device %u: %s\n", gpu_index, p_gpu_info->properties.device_name);

    const char* type = "unknown";
    switch(p_gpu_info->properties.device_type)
    {
        case vk_device_type__integrated_gpu:
            type = "integrated gpu";
            break;
        case vk_device_type__discrete_gpu:
            type = "discrete gpu";
            break;
        case vk_device_type__virtual_gpu:
            type = "virtual gpu";
            break;
        case vk_device_type__cpu:
            type = "cpu";
            break;
        default:
            break;
    }
    printf("\ttype: %s\n", type);

    printf("\tvendor: 0x%X\n", p_gpu_info->properties.vendor_id);
    printf("\tdevice id: 0x%X\n", p_gpu_info->properties.device_id);
    printf("\tdriver version:0x%X\n", p_gpu_info->properties.driver_version);
    printf("\tapi version version:0x%X\n", p_gpu_info->properties.api_version);

    printf("\tFeatures:\n");
    if(p_gpu_info->features.robust_buffer_access) { printf("\t\tRobust buffer access\n"); }
    if(p_gpu_info->features.full_draw_index_uint32) { printf("\t\tFull draw index\n"); }
    if(p_gpu_info->features.image_cube_array) { printf("\t\tImage cube array\n"); }
    if(p_gpu_info->features.independent_blend) { printf("\t\tIndependent blend\n"); }
    if(p_gpu_info->features.geometry_shader) { printf("\t\tGeometry shader\n"); }
    if(p_gpu_info->features.tessellation_shader) { printf("\t\tTessellation shader\n"); }
    if(p_gpu_info->features.sample_rate_shading) { printf("\t\tSample rate shading\n"); }
    if(p_gpu_info->features.dual_src_blend) { printf("\t\tDual source blend\n"); }
    if(p_gpu_info->features.logic_op) { printf("\t\tLogic op\n"); }
    if(p_gpu_info->features.multi_draw_indirect) { printf("\t\tMulti draw indirect\n"); }
    if(p_gpu_info->features.draw_indirect_first_instance) { printf("\t\tDraw indirect first instance\n"); }
    if(p_gpu_info->features.depth_clamp) { printf("\t\tDepth clamp\n"); }
    if(p_gpu_info->features.depth_bias_clamp) { printf("\t\tDepth bias clamp\n"); }
    if(p_gpu_info->features.fill_mode_non_solid) { printf("\t\tFill mode non solid\n"); }
    if(p_gpu_info->features.depth_bounds) { printf("\t\tDepth bounds\n"); }
    if(p_gpu_info->features.wide_lines) { printf("\t\tWide lines\n"); }
    if(p_gpu_info->features.large_points) { printf("\t\tLarge points\n"); }
    if(p_gpu_info->features.alpha_to_one) { printf("\t\tAlpha to one\n"); }
    if(p_gpu_info->features.multi_viewport) { printf("\t\tMulti viewport\n"); }
    if(p_gpu_info->features.sampler_anisotropy) { printf("\t\tSampler anisotropy\n"); }
    if(p_gpu_info->features.texture_compression_etc2) { printf("\t\tTexture compression etc2\n"); }
    if(p_gpu_info->features.texture_compression_astc_ldr) { printf("\t\tTexture compression astc ldr\n"); }
    if(p_gpu_info->features.texture_compression_bc) { printf("\t\tTexture compression bc\n"); }
    if(p_gpu_info->features.occlusion_query_precise) { printf("\t\tOcclusion query precise\n"); }
    if(p_gpu_info->features.pipeline_statistics_query) { printf("\t\tPipeline statistics query\n"); }
    if(p_gpu_info->features.vertex_pipeline_stores_and_atomics) { printf("\t\tVertex pipeline stores and atomics\n"); }
    if(p_gpu_info->features.fragment_stores_and_atomics) { printf("\t\tFragment stores and atomics\n"); }
    if(p_gpu_info->features.shader_tessellation_and_geometry_point_size) { printf("\t\tShader tessellation and geometry point size\n"); }
    if(p_gpu_info->features.shader_image_gather_extended) { printf("\t\tShader image gather extended\n"); }
    if(p_gpu_info->features.shader_storage_image_extended_formats) { printf("\t\tShader storage image extended formats\n"); }
    if(p_gpu_info->features.shader_storage_image_multisample) { printf("\t\tShader storage image multisample\n"); }
    if(p_gpu_info->features.shader_storage_image_read_without_format) { printf("\t\tShader storage image read without format\n"); }
    if(p_gpu_info->features.shader_storage_image_write_without_format) { printf("\t\tShader storage image write without format\n"); }
    if(p_gpu_info->features.shader_uniform_buffer_array_dynamic_indexing) { printf("\t\tShader uniform buffer array dynamic indexing\n"); }
    if(p_gpu_info->features.shader_sampled_image_array_dynamic_indexing) { printf("\t\tShader sampled image array dynamic indexing\n"); }
    if(p_gpu_info->features.shader_storage_buffer_array_dynamic_indexing) { printf("\t\tShader storage buffer array dynamic indexing\n"); }
    if(p_gpu_info->features.shader_storage_image_array_dynamic_indexing) { printf("\t\tShader storage image array dynamic indexing\n"); }
    if(p_gpu_info->features.shader_clip_distance) { printf("\t\tShader clip distance\n"); }
    if(p_gpu_info->features.shader_cull_distance) { printf("\t\tShader cull distance\n"); }
    if(p_gpu_info->features.shader_float64) { printf("\t\tShader float 64\n"); }
    if(p_gpu_info->features.shader_int64) { printf("\t\tShader int 64\n"); }
    if(p_gpu_info->features.shader_int16) { printf("\t\tShader int 16\n"); }
    if(p_gpu_info->features.shader_resource_residency) { printf("\t\tShader resource residency\n"); }
    if(p_gpu_info->features.shader_resource_min_lod) { printf("\t\tShader resource min lod\n"); }
    if(p_gpu_info->features.sparse_binding) { printf("\t\tSparse binding\n"); }
    if(p_gpu_info->features.sparse_residency_buffer) { printf("\t\tSparse residency buffer\n"); }
    if(p_gpu_info->features.sparse_residency_image2D) { printf("\t\tSparse residency image 2D\n"); }
    if(p_gpu_info->features.sparse_residency_image3D) { printf("\t\tSparse residency image 3D\n"); }
    if(p_gpu_info->features.sparse_residency2_samples) { printf("\t\tSparse residency 2 samples\n"); }
    if(p_gpu_info->features.sparse_residency4_samples) { printf("\t\tSparse residency 4 samples\n"); }
    if(p_gpu_info->features.sparse_residency8_samples) { printf("\t\tSparse residency 8 samples\n"); }
    if(p_gpu_info->features.sparse_residency16_samples) { printf("\t\tSparse residency 16 samples\n"); }
    if(p_gpu_info->features.sparse_residency_aliased) { printf("\t\tSparse residency aliased\n"); }
    if(p_gpu_info->features.variable_multisample_rate) { printf("\t\tVariable multi-sample rate\n"); }
    if(p_gpu_info->features.inherited_queries) { printf("\t\tInherited queries\n"); }

    for(uint32_t i = 0; i < p_gpu_info->queue_group_count; i++)
    {
        printf("\tqueue group %u:\n", i);
        printf("\t\tqueue count: %u\n", p_gpu_info->p_queue_group_properties[i].queue_count);
        
        if(p_gpu_info->p_queue_group_properties[i].queue_flags.graphics) { printf("\t\tgraphics supported\n"); }
        if(p_gpu_info->p_queue_group_properties[i].queue_flags.compute) { printf("\t\tcompute supported\n"); }
        if(p_gpu_info->p_queue_group_properties[i].queue_flags.transfer) { printf("\t\ttransfer supported\n"); }
        if(p_gpu_info->p_queue_group_properties[i].queue_flags.sparse_binding) { printf("\t\tsparse_binding supported\n"); }
        if(p_gpu_info->p_queue_group_properties[i].queue_flags.protected_memory) { printf("\t\tprotected_memory supported\n"); }
        if(p_gpu_info->p_queue_group_properties[i].queue_flags.video_decode) { printf("\t\tvideo_decode supported\n"); }
        if(p_gpu_info->p_queue_group_properties[i].queue_flags.video_encode) { printf("\t\tvideo_encode supported\n"); }
        if(p_gpu_info->p_queue_group_properties[i].queue_flags.optical_flow) { printf("\t\toptical_flow supported\n"); }
    }
}

bool get_gpu_queue_info(vk_physical_device h_physical_device, uint32_t* p_num_queue_groups, struct vk_queue_group_properties** p_queue_group_properties)
{
    bool status = true;

    uint32_t num_groups = 0;
    struct vk_queue_group_properties* p_groups = NULL;

    vk_ctx.get_physical_queue_group_properties(h_physical_device, &num_groups, NULL);

    if(num_groups > 0)
    {
        p_groups = malloc(num_groups * sizeof(struct vk_queue_group_properties));

        if(p_groups == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(num_groups > 0)
        {
            vk_ctx.get_physical_queue_group_properties(h_physical_device, &num_groups, p_groups);
        }

        *p_num_queue_groups = num_groups;
        *p_queue_group_properties = p_groups;
    }

    return status;
}

bool enumerate_device_layers_and_extensions(vk_physical_device h_physical_device)
{
    bool status = true;

    uint32_t num_layers = 0;
    struct vk_layer* p_layers = NULL;

    if(vk_ctx.enumerate_device_layers(h_physical_device, &num_layers, NULL) != vk_success)
    {
        status = false;
        printf("failed to get number of layers\n");
    }

    if(status)
    {
        p_layers = malloc(num_layers * sizeof(struct vk_layer));

        if (p_layers == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(vk_ctx.enumerate_device_layers(h_physical_device, &num_layers, p_layers) != vk_success)
        {
            status = false;
            printf("failed to get layers\n");
        }
    }

    if(status)
    {
        // enumerate the extensions provided by the vulkan implementation
        printf("Device layer: Vulkan implementation\n");
        status = enumerate_device_extensions(h_physical_device, NULL);
    }

    if(status)
    {
        for(uint32_t i = 0; i < num_layers; i++)
        {
            printf("Device layer %s (0x%X, %u): %s\n", p_layers[i].name, p_layers[i].spec_version, p_layers[i].impl_version, p_layers[i].desc);
            status = enumerate_device_extensions(h_physical_device, p_layers[i].name);
        }
    }

    if(p_layers != NULL)
    {
        free(p_layers);
        p_layers = NULL;
    }

    return status;
}

bool enumerate_device_extensions(vk_physical_device h_physical_device, const char* p_layer)
{
    bool status = true;

    uint32_t num_extensions = 0;
    struct vk_extension* p_extensions = NULL;

    if(vk_ctx.enumerate_device_extensions(h_physical_device, p_layer, &num_extensions, NULL) != vk_success)
    {
        status = false;
        printf("failed to get number of extensions\n");
    }

    if(status)
    {
        p_extensions = malloc(num_extensions * sizeof(struct vk_extension));

        if(p_extensions == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(vk_ctx.enumerate_device_extensions(h_physical_device, p_layer, &num_extensions, p_extensions) != vk_success)
        {
            status = false;
            printf("failed to get extensions\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < num_extensions; i++)
        {
            printf("\tDevice extension: %s\n", p_extensions[i].name);
        }
    }

    if(p_extensions != NULL)
    {
        free(p_extensions);
        p_extensions = NULL;
    }

    return status;
}
