
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vk_context.h"

struct vk_context vk_ctx = { 0 };

struct physical_device_info
{
    vk_physical_device                   handle;
    struct vk_physical_device_properties properties;

    uint32_t                             queue_group_count;
    struct vk_queue_group_properties*    p_queue_group_properties;
};

bool initialize_global_function_pointers(void);
bool initialize_instance_function_pointers(void);
bool initialize_instance(void);
bool initialize_device(void);

bool enumerate_layers(void);
bool enumerate_extensions(const char* p_layer);
bool enumerate_devices_and_queues(uint32_t* p_device_count, struct physical_device_info** p_device_info);

bool get_physical_device_queue_info(vk_physical_device h_physical_device, uint32_t* p_num_queue_groups, struct vk_queue_group_properties** p_queue_group_properties);

void print_physical_device_info(uint32_t device_index, struct vk_physical_device_properties* p_device_properties, uint32_t num_queue_groups, struct vk_queue_group_properties* p_queue_group_properties);

void free_physical_device_info(uint32_t p_device_count, struct physical_device_info* p_device_info);

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
        status = enumerate_layers();
    }

    if(status)
    {
        status = initialize_instance();
    }

    if(status)
    {
        status = initialize_instance_function_pointers();
    }

    if(status)
    {
        status = initialize_device();
    }

    return status;
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

    status &= load_function_pointer(vk_ctx.h_instance, "vkEnumeratePhysicalDevices", (void**) &vk_ctx.enumerate_devices);
    status &= load_function_pointer(vk_ctx.h_instance, "vkGetPhysicalDeviceProperties", (void**) &vk_ctx.get_physical_device_properties);
    status &= load_function_pointer(vk_ctx.h_instance, "vkGetPhysicalDeviceQueueFamilyProperties", (void**) &vk_ctx.get_physical_queue_group_properties);
    status &= load_function_pointer(vk_ctx.h_instance, "vkCreateDevice", (void**) &vk_ctx.create_device);

    return status;
}

bool enumerate_layers(void)
{
    bool status = true;

    uint32_t num_layers = 0;
    struct vk_layer* p_layers = NULL;

    if(vk_ctx.enumerate_layers(&num_layers, NULL) != vk_success)
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
        if(vk_ctx.enumerate_layers(&num_layers, p_layers) != vk_success)
        {
            status = false;
            printf("failed to get layers\n");
        }
    }

    if(status)
    {
        // enumerate the extensions provided by the vulkan implementation
        printf("Layer: Vulkan implementation\n");
        status = enumerate_extensions(NULL);
    }

    if(status)
    {
        for(uint32_t i = 0; i < num_layers; i++)
        {
            printf("Layer %s (0x%X, %u): %s\n", p_layers[i].name, p_layers[i].spec_version, p_layers[i].impl_version, p_layers[i].desc);
            status = enumerate_extensions(p_layers[i].name);
        }
    }

    if(p_layers != NULL)
    {
        free(p_layers);
        p_layers = NULL;
    }

    return status;
}

bool enumerate_extensions(const char* p_layer)
{
    bool status = true;

    uint32_t num_extensions = 0;
    struct vk_extension* p_extensions = NULL;

    if(vk_ctx.enumerate_extensions(p_layer, &num_extensions, NULL) != vk_success)
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
        if(vk_ctx.enumerate_extensions(p_layer, &num_extensions, p_extensions) != vk_success)
        {
            status = false;
            printf("failed to get extensions\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < num_extensions; i++)
        {
            printf("\tExtension: %s\n", p_extensions[i].name);
        }
    }

    if(p_extensions != NULL)
    {
        free(p_extensions);
        p_extensions = NULL;
    }

    return status;
}

bool initialize_instance(void)
{
    bool status = true;

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
    instance_info.extension_count = 0;
    instance_info.p_extension_names = NULL;

    if(vk_ctx.create_instance(&instance_info, NULL, &vk_ctx.h_instance) != vk_success)
    {
        status = false;
        printf("failed to create vulkan instance\n");
    }

    printf("instance: 0x%p\n", vk_ctx.h_instance);
    
    return status;
}

bool enumerate_devices_and_queues(uint32_t* p_device_count, struct physical_device_info** p_device_info)
{
    bool status = true;

    uint32_t num_physical_devices = 0;

    vk_physical_device* p_physical_device_handles = NULL;

    struct physical_device_info *p_physical_device_info = NULL;

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
        p_physical_device_info = malloc(num_physical_devices * sizeof(struct physical_device_info));

        if(p_physical_device_info == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        for(uint32_t i = 0; status && (i < num_physical_devices); i++)
        {
            p_physical_device_info[i].handle = p_physical_device_handles[i];
            vk_ctx.get_physical_device_properties(p_physical_device_handles[i], &p_physical_device_info[i].properties);
            
            status = get_physical_device_queue_info(p_physical_device_handles[i], &p_physical_device_info[i].queue_group_count, &p_physical_device_info[i].p_queue_group_properties);
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < num_physical_devices; i++)
        {
            print_physical_device_info(i, &p_physical_device_info[i].properties, p_physical_device_info[i].queue_group_count, p_physical_device_info[i].p_queue_group_properties);
        }
    }

    if (status)
    {
        *p_device_count = num_physical_devices;
        *p_device_info = p_physical_device_info;
    }
    else
    {
        free_physical_device_info(num_physical_devices, p_physical_device_info);
        p_physical_device_info = NULL;
    }

    if(p_physical_device_handles != NULL)
    {
        free(p_physical_device_handles);
        p_physical_device_handles = NULL;
    }

    return status;
}

void free_physical_device_info(uint32_t p_device_count, struct physical_device_info* p_device_info)
{
    if(p_device_info != NULL)
    {
        for(uint32_t i = 0; i < p_device_count; i++)
        {
            if(p_device_info[i].p_queue_group_properties != NULL)
            {
                free(p_device_info[i].p_queue_group_properties);
                p_device_info[i].p_queue_group_properties = NULL;
            }
        }

        free(p_device_info);
        p_device_info = NULL;
    }
}

bool initialize_device(void)
{
    bool status = true;

    uint32_t device_count = 0;
    uint32_t device_index = 0;
    uint32_t queue_group_index = 0;
    struct physical_device_info* p_device_info = NULL;

    if(status)
    {
        status = enumerate_devices_and_queues(&device_count, &p_device_info);
    }

    if(status)
    {
        while(device_index < device_count)
        {
            if(p_device_info[device_index].properties.device_type == vk_device_type__discrete_gpu)
            {
                printf("use device %u\n", device_index);
                break;
            }

            device_index++;
        }

        if(device_index >= device_count)
        {
            status = false;
            printf("error: could not find discrete gpu\n");
        }
    }

    if(status)
    {
        while(queue_group_index < p_device_info[device_index].queue_group_count)
        {
            if(p_device_info[device_index].p_queue_group_properties[queue_group_index].queue_flags.graphics)
            {
                printf("use queue group %u\n", queue_group_index);
                break;
            }

            queue_group_index++;
        }

        if(queue_group_index >= p_device_info[device_index].queue_group_count)
        {
            status = false;
            printf("error: could not find queue group\n");
        }
    }

#if 0
    if(status)
    {
        const uint32_t MAX_QUEUES = 2;

        const float p_queue_priorities[MAX_QUEUES] = { 1.0f, 1.0f };

        uint32_t queue_count = p_physical_device_info[device_index].p_queue_group_properties[queue_group_index].queue_count;

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
        device_info.

        vk_ctx.create_device(p_physical_devices[device_index], )
    }
#endif

    free_physical_device_info(device_count, p_device_info);
    p_device_info = NULL;

    return status;
}

void print_physical_device_info(uint32_t device_index, struct vk_physical_device_properties* p_device_properties, uint32_t num_queue_groups, struct vk_queue_group_properties* p_queue_group_properties)
{
    printf("device %u: %s\n", device_index, p_device_properties->device_name);

    const char* type = "unknown";
    switch(p_device_properties->device_type)
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

    printf("\tvendor: 0x%X\n", p_device_properties->vendor_id);
    printf("\tdevice id: 0x%X\n", p_device_properties->device_id);
    printf("\tdriver version:0x%X\n", p_device_properties->driver_version);
    printf("\tapi version version:0x%X\n", p_device_properties->api_version);

    for(uint32_t i = 0; i < num_queue_groups; i++)
    {
        printf("\tqueue group %u:\n", i);
        printf("\t\tqueue count: %u\n", p_queue_group_properties[i].queue_count);
        
        if(p_queue_group_properties[i].queue_flags.graphics) { printf("\t\tgraphics supported\n"); }
        if(p_queue_group_properties[i].queue_flags.compute) { printf("\t\tcompute supported\n"); }
        if(p_queue_group_properties[i].queue_flags.transfer) { printf("\t\ttransfer supported\n"); }
        if(p_queue_group_properties[i].queue_flags.sparse_binding) { printf("\t\tsparse_binding supported\n"); }
        if(p_queue_group_properties[i].queue_flags.protected_memory) { printf("\t\tprotected_memory supported\n"); }
        if(p_queue_group_properties[i].queue_flags.video_decode) { printf("\t\tvideo_decode supported\n"); }
        if(p_queue_group_properties[i].queue_flags.video_encode) { printf("\t\tvideo_encode supported\n"); }
        if(p_queue_group_properties[i].queue_flags.optical_flow) { printf("\t\toptical_flow supported\n"); }
    }
}

bool get_physical_device_queue_info(vk_physical_device h_physical_device, uint32_t* p_num_queue_groups, struct vk_queue_group_properties** p_queue_group_properties)
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

