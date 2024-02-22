#ifndef VK_INTERFACE_H
#define VK_INTERFACE_H

#include "vk.h"

struct vk_context
{
    vk_instance                                  h_instance;
    vk_device                                    h_device;

    pfn_vk_get_instance_proc_addr                get_instance_proc_addr;

    // global functions
    pfn_vk_get_version                           get_version;
    pfn_vk_create_instance                       create_instance;
    pfn_vk_enumerate_layers                      enumerate_layers;
    pfn_vk_enumerate_extensions                  enumerate_extensions;

    // instance level functions
    pfn_vk_enumerate_physical_devices            enumerate_devices;
    pfn_vk_get_physical_device_properties        get_physical_device_properties;
    pfn_vk_get_physical_device_features          get_physical_device_features;
    pfn_vk_get_physical_queue_group_properties   get_physical_queue_group_properties;
    pfn_vk_create_device                         create_device;

    // device level functions
    pfn_vk_enumerate_device_layers               enumerate_device_layers;
    pfn_vk_enumerate_device_extensions           enumerate_device_extensions;
};

extern struct vk_context vk_ctx;

bool initialize_vulkan_context(pfn_vk_get_instance_proc_addr pfn_get_instance_proc_addr);

#endif // VK_INTERFACE_H
