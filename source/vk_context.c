
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vk_context.h"

struct vk_context vk_ctx = { 0 };

struct physical_device_info
{
    struct vk_physical_device_properties       device_properties;

    uint32_t                                   num_queue_groups;
    struct vk_physical_queue_group_properties* p_queue_group_properties;
};

bool initialize_global_function_pointers(void);
bool initialize_instance_function_pointers(void);
bool initialize_instance(void);

bool enumerate_layers(void);
bool enumerate_extensions(const char* p_layer);
bool enumerate_devices(void);

void print_physical_device_info(struct physical_device_info* p_devices, uint32_t index);
bool get_physical_device_queue_info(vk_physical_device h_device, struct physical_device_info* p_info);

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
        status = enumerate_devices();
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
    app_info.type = vk_application_info;
    app_info.p_next = NULL;
    app_info.p_app_name = "vk-cube";
    app_info.app_version = 1;
    app_info.p_engine_name = "vk-cube";
    app_info.engine_version = 1;
    app_info.api_version = VK_VERSION(0, 1, 0, 0);

    struct vk_instance_info instance_info = { 0 };
    instance_info.type = vk_instance_info;
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

    printf("instance: %p\n", vk_ctx.h_instance);
    
    return status;
}

bool enumerate_devices(void)
{
    bool status = true;

    uint32_t num_physical_devices = 0;
    vk_physical_device* p_physical_devices = NULL;

    struct physical_device_info* p_physical_device_info = NULL;

    if(vk_ctx.enumerate_devices(vk_ctx.h_instance, &num_physical_devices, NULL) != vk_success)
    {
        status = false;
        printf("failed to get number of devices\n");
    }

    if(status)
    {
        p_physical_devices = malloc(num_physical_devices * sizeof(vk_physical_device));

        if(p_physical_devices == NULL)
        {
            status = false;
            printf("failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(vk_ctx.enumerate_devices(vk_ctx.h_instance, &num_physical_devices, p_physical_devices) != vk_success)
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
            vk_ctx.get_physical_device_properties(p_physical_devices[i], &p_physical_device_info[i].device_properties);
            
            status = get_physical_device_queue_info(p_physical_devices[i], &p_physical_device_info[i]);
        }
    }

    if(status)
    {
        for(uint32_t i = 0; i < num_physical_devices; i++)
        {
            print_physical_device_info(p_physical_device_info, i);
        }
    }

    if(status)
    {
        uint32_t device_index = 0;

        while(device_index < num_physical_devices)
        {
            if(p_physical_device_info[device_index].device_properties.device_type == vk_device_type__discrete_gpu)
            {
                printf("use device %u\n", device_index);
                break;
            }

            device_index++;
        }

        if(device_index >= num_physical_devices)
        {
            status = false;
            printf("error: could not find discrete gpu\n");
        }
    }

    if(p_physical_device_info != NULL)
    {
        for(uint32_t i = 0; i < num_physical_devices; i++)
        {
            if(p_physical_device_info[i].p_queue_group_properties != NULL)
            {
                free(p_physical_device_info[i].p_queue_group_properties);
                p_physical_device_info[i].p_queue_group_properties = NULL;
            }
        }

        free(p_physical_device_info);
        p_physical_device_info = NULL;
    }

    if(p_physical_devices != NULL)
    {
        free(p_physical_devices);
        p_physical_devices = NULL;
    }

    return status;
}

void print_physical_device_info(struct physical_device_info* p_physical_devices, uint32_t index)
{
    printf("device %u: %s\n", index, p_physical_devices[index].device_properties.device_name);

    const char* type = "unknown";
    switch(p_physical_devices[index].device_properties.device_type)
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

    printf("\tvendor: 0x%X\n", p_physical_devices[index].device_properties.vendor_id);
    printf("\tdevice id: 0x%X\n", p_physical_devices[index].device_properties.device_id);
    printf("\tdriver version:0x%X\n", p_physical_devices[index].device_properties.driver_version);
    printf("\tapi version version:0x%X\n", p_physical_devices[index].device_properties.api_version);

    for(uint32_t i = 0; i < p_physical_devices[index].num_queue_groups; i++)
    {
        printf("\tqueue group %u:\n", i);
        printf("\t\tqueue count: %u\n", p_physical_devices[index].p_queue_group_properties[i].queue_count);
        
        if(p_physical_devices[index].p_queue_group_properties[i].queue_flags.graphics) { printf("\t\tgraphics supported\n"); }
        if(p_physical_devices[index].p_queue_group_properties[i].queue_flags.compute) { printf("\t\tcompute supported\n"); }
        if(p_physical_devices[index].p_queue_group_properties[i].queue_flags.transfer) { printf("\t\ttransfer supported\n"); }
        if(p_physical_devices[index].p_queue_group_properties[i].queue_flags.sparse_binding) { printf("\t\tsparse_binding supported\n"); }
        if(p_physical_devices[index].p_queue_group_properties[i].queue_flags.protected_memory) { printf("\t\tprotected_memory supported\n"); }
        if(p_physical_devices[index].p_queue_group_properties[i].queue_flags.video_decode) { printf("\t\tvideo_decode supported\n"); }
        if(p_physical_devices[index].p_queue_group_properties[i].queue_flags.video_encode) { printf("\t\tvideo_encode supported\n"); }
        if(p_physical_devices[index].p_queue_group_properties[i].queue_flags.optical_flow) { printf("\t\toptical_flow supported\n"); }
    }
}

bool get_physical_device_queue_info(vk_physical_device h_physical_device, struct physical_device_info* p_info)
{
    bool status = true;

    uint32_t num_groups = 0;
    struct vk_physical_queue_group_properties* p_groups = NULL;

    vk_ctx.get_physical_queue_group_properties(h_physical_device, &num_groups, NULL);

    if(num_groups > 0)
    {
        p_groups = malloc(num_groups * sizeof(struct vk_physical_queue_group_properties));

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

        p_info->num_queue_groups = num_groups;
        p_info->p_queue_group_properties = p_groups;
    }

    return status;
}
