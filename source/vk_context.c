
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vk_context.h"

struct vk_context vk_ctx = { 0 };

bool initialize_global_function_pointers(void);
bool initialize_instance_function_pointers(void);
bool initialize_instance(void);

bool enumerate_layers(void);
bool enumerate_extensions(const char* layer);

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

    return status;
}

bool load_function_pointer(vk_instance instance, const char* name, void** pfn)
{
    bool status = true;

    if(pfn != NULL)
    {
        (*pfn) = vk_ctx.get_instance_proc_addr(instance, name);

        if((*pfn) == NULL)
        {
            status = false;
            printf("error: failed to load function pointer %s\n", name);
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

    status &= load_function_pointer(vk_ctx.instance, "vkEnumeratePhysicalDevices", (void**) &vk_ctx.enumerate_devices);

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

bool enumerate_extensions(const char* layer)
{
    bool status = true;

    uint32_t num_extensions = 0;
    struct vk_extension* p_extensions = NULL;

    if(vk_ctx.enumerate_extensions(layer, &num_extensions, NULL) != vk_success)
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
        if(vk_ctx.enumerate_extensions(layer, &num_extensions, p_extensions) != vk_success)
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
    app_info.next = NULL;
    app_info.app_name = "vk-cube";
    app_info.app_version = 1;
    app_info.engine_name = "vk-cube";
    app_info.engine_version = 1;
    app_info.api_version = VK_VERSION(0, 1, 0, 0);

    struct vk_instance_info instance_info = { 0 };
    instance_info.type = vk_instance_info;
    instance_info.next = NULL;
    instance_info.app_info = &app_info;
    instance_info.layer_count = 0;
    instance_info.layer_names = NULL;
    instance_info.extension_count = 0;
    instance_info.extension_names = NULL;

    if(vk_ctx.create_instance(&instance_info, NULL, &vk_ctx.instance) != vk_success)
    {
        status = false;
        printf("failed to create vulkan instance\n");
    }

    printf("instance: %p\n", vk_ctx.instance);
    
    return status;
}

