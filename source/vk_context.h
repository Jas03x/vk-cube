#ifndef VK_INTERFACE_H
#define VK_INTERFACE_H

#include "vk.h"

struct vk_context
{
    vk_instance                   instance;

    pfn_vk_get_instance_proc_addr get_instance_proc_addr;

    pfn_vk_get_version            get_version;
    pfn_vk_create_instance        create_instance;
    pfn_vk_get_layers             get_layers;
};

extern struct vk_context vk_ctx;

bool initialize_vulkan_context(pfn_vk_get_instance_proc_addr pfn_get_instance_proc_addr);

#endif // VK_INTERFACE_H
