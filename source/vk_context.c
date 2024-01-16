
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "vk_context.h"

struct vk_context vk_ctx = { 0 };

bool initialize_function_pointers(void);
bool initialize_instance(void);

bool initialize_vulkan_context(pfn_vk_get_instance_proc_addr pfn_get_instance_proc_addr)
{
    bool status = true;

    vk_ctx.get_instance_proc_addr = pfn_get_instance_proc_addr;

    if(status)
    {
        status = initialize_function_pointers();
    }

    if (status)
    {
        status = initialize_instance();
    }

    return status;
}

bool initialize_function_pointers(void)
{
    bool status = true;

    // get global functions
    vk_ctx.create_instance = vk_ctx.get_instance_proc_addr(NULL, "vkCreateInstance");
    vk_ctx.get_version = vk_ctx.get_instance_proc_addr(NULL, "vkEnumerateInstanceVersion");
    vk_ctx.get_layers = vk_ctx.get_instance_proc_addr(NULL, "vkEnumerateInstanceLayerProperties");

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

    vk_ctx.create_instance(&instance_info, NULL, &vk_ctx.instance);

    printf("instance: %p\n", vk_ctx.instance);
    
    return status;
}

