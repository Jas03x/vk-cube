#ifndef VK_INTERFACE_H
#define VK_INTERFACE_H

#include "vk.h"

struct vk_context
{
    vk_instance                                      h_instance;
    vk_device                                        h_device; // logical device
    vk_physical_device                               h_physical_device;
    vk_swapchain                                     h_swapchain;

    uint32_t                                         graphics_queue_family;

    vk_debug_callback                                h_debug_callback;

    pfn_vk_get_instance_proc_addr                    get_instance_proc_addr;
    pfn_vk_get_device_proc_addr                      get_device_proc_addr;

    // global functions
    pfn_vk_create_instance                           create_instance;
    pfn_vk_destroy_instance                          destroy_instance;
    pfn_vk_get_version                               get_version;
    pfn_vk_enumerate_layers                          enumerate_layers;
    pfn_vk_enumerate_extensions                      enumerate_extensions;

    // instance level functions
    pfn_vk_enumerate_physical_devices                enumerate_devices;
    pfn_vk_get_physical_device_properties            get_physical_device_properties;
    pfn_vk_get_physical_device_features              get_physical_device_features;
    pfn_vk_get_physical_queue_group_properties       get_physical_queue_group_properties;
    pfn_vk_create_device                             create_device;
    pfn_vk_destroy_device                            destroy_device;
    pfn_vk_wait_for_device_idle                      wait_for_device_idle;
    pfn_vk_register_debug_callback                   register_debug_callback;
    pfn_vk_unregister_debug_callback                 unregister_debug_callback;
    pfn_vk_get_physical_device_surface_support       get_physical_device_surface_support;
    pfn_vk_get_physical_device_surface_capabilities  get_physical_device_surface_capabilities;
    pfn_vk_get_physical_device_surface_present_modes get_physical_device_surface_present_modes;
    pfn_vk_get_physical_device_surface_formats       get_physical_device_surface_formats;
    pfn_vk_create_swapchain                          create_swapchain;
    pfn_vk_enumerate_device_layers                   enumerate_device_layers;
    pfn_vk_enumerate_device_extensions               enumerate_device_extensions;
    pfn_vk_create_semaphore                          create_semaphore;

    // device level functions
    pfn_vk_acquire_next_image                        acquire_next_image;
};

extern struct vk_context* vk_ctx;

bool initialize_vulkan_context(pfn_vk_get_instance_proc_addr pfn_get_instance_proc_addr, uint32_t ext_count, const char** ext_array);
void uninitialize_vulkan_context(void);

bool initialize_swapchain(vk_surface surface);

bool present(void);

#endif // VK_INTERFACE_H
