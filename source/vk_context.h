#ifndef VK_INTERFACE_H
#define VK_INTERFACE_H

#include <vulkan/vulkan.h>

enum { VK_CTX_NUM_GRAPHICS_QUEUES   = 1 };
enum { VK_CTX_NUM_SWAPCHAIN_BUFFERS = 2 };

struct vk_context
{
    VkInstance                                       instance;

    VkAllocationCallbacks*                           allocation_callbacks;

    VkDevice                                         device;
    VkPhysicalDevice                                 physical_device;

    VkSurfaceKHR                                     surface;
    VkFormat                                         surface_format;
    
    VkSwapchainKHR                                   swapchain;
    VkImage                                          swapchain_images[VK_CTX_NUM_SWAPCHAIN_BUFFERS];

    VkCommandPool                                    command_pool;
    VkCommandBuffer                                  command_buffer;

    VkQueue                                          graphics_queues[VK_CTX_NUM_GRAPHICS_QUEUES];

    VkSemaphore                                      image_available_semaphore;
    VkSemaphore                                      rendering_finished_semaphore;

    uint32_t                                         graphics_queue_family;

    VkDebugReportCallbackEXT                         debug_callback;

    PFN_vkGetInstanceProcAddr                        get_instance_proc_addr;
    PFN_vkGetDeviceProcAddr                          get_device_proc_addr;

    // global functions
    PFN_vkCreateInstance                             create_instance;
    PFN_vkDestroyInstance                            destroy_instance;
    PFN_vkEnumerateInstanceVersion                   enumerate_instance_version;
    PFN_vkEnumerateInstanceLayerProperties           enumerate_instance_layers;
    PFN_vkEnumerateInstanceExtensionProperties       enumerate_instance_extensions;

    // instance level functions
    PFN_vkEnumeratePhysicalDevices                   enumerate_physical_devices;
    PFN_vkGetPhysicalDeviceProperties                get_physical_device_properties;
    PFN_vkGetPhysicalDeviceFeatures                  get_physical_device_features;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties     get_physical_queue_group_properties;
    PFN_vkCreateDevice                               create_device;
    PFN_vkCreateDebugReportCallbackEXT               register_debug_callback;
    PFN_vkDestroyDebugReportCallbackEXT              unregister_debug_callback;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR         get_physical_device_surface_support;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR    get_physical_device_surface_capabilities;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR    get_physical_device_surface_present_modes;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR         get_physical_device_surface_formats;
    PFN_vkEnumerateDeviceLayerProperties             enumerate_device_layers;
    PFN_vkEnumerateDeviceExtensionProperties         enumerate_device_extensions;
    PFN_vkDestroySurfaceKHR                          destroy_surface;

    // device level functions
    PFN_vkCreateSemaphore                            create_semaphore;
    PFN_vkDestroySemaphore                           destroy_semaphore;
    PFN_vkCreateSwapchainKHR                         create_swapchain;
    PFN_vkDestroySwapchainKHR                        destroy_swapchain;
    PFN_vkCreateCommandPool                          create_command_pool;
    PFN_vkDestroyCommandPool                         destroy_command_pool;
    PFN_vkAllocateCommandBuffers                     allocate_command_buffers;
    PFN_vkFreeCommandBuffers                         free_command_buffers;
    PFN_vkAcquireNextImageKHR                        acquire_next_image;
    PFN_vkDeviceWaitIdle                             wait_for_device_idle;
    PFN_vkDestroyDevice                              destroy_device;
    PFN_vkGetSwapchainImagesKHR                      get_swapchain_images;
    PFN_vkBeginCommandBuffer                         begin_command_buffer;
    PFN_vkEndCommandBuffer                           end_command_buffer;
    PFN_vkCmdPipelineBarrier                         cmd_pipeline_barrier;
    PFN_vkCmdClearColorImage                         cmd_clear_color_image;
    PFN_vkQueueSubmit                                queue_submit;
    PFN_vkQueuePresentKHR                            queue_present;
    PFN_vkGetDeviceQueue                             get_device_queue;
    PFN_vkCreateRenderPass                           create_render_pass;
    PFN_vkCreateImageView                            create_image_view;
    PFN_vkCreateFramebuffer                          create_framebuffer;
    PFN_vkCreateShaderModule                         create_shader_module;
    PFN_vkCreatePipelineLayout                       create_pipeline_layout;
    PFN_vkCreateGraphicsPipelines                    create_graphics_pipelines;
};

extern struct vk_context* vk_ctx;

bool initialize_vulkan_context(PFN_vkGetInstanceProcAddr pfn_get_instance_proc_addr, uint32_t ext_count, const char** ext_array);
void uninitialize_vulkan_context(void);

bool initialize_swapchain(VkSurfaceKHR surface);

#endif // VK_INTERFACE_H
