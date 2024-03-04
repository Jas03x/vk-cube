#ifndef VK_H
#define VK_H

#define VK_MAX_NAME_LENGTH 256
#define VK_MAX_DESC_LENGTH 256
#define VK_MAX_DEVICE_NAME_LENGTH 256
#define VK_UUID_SIZE 16

#define VK_VERSION(variant, major, minor, patch) ((((variant) & 0x3) << 29) | (((major) & 0x7F) << 22) | (((minor) & 0x3FF) << 12) | ((patch) & 0xFFF))

typedef void* vk_fence;
typedef void* vk_instance;
typedef void* vk_physical_device;
typedef void* vk_device;
typedef void* vk_debug_callback;
typedef void* vk_semaphore;
typedef void* vk_surface;
typedef void* vk_swapchain;
typedef void* vk_command_pool;
typedef void* vk_command_buffer;
typedef void* vk_render_pass;
typedef void* vk_framebuffer;

enum
{
    vk_false = 0,
    vk_true  = 1
};

enum vk_result
{
    vk_success = 0
};

enum vk_structure_type
{
    vk_structure_type__application_info             = 0,
    vk_structure_type__instance_info                = 1,
    vk_structure_type__queue_create_info            = 2,
    vk_structure_type__device_create_info           = 3,
    vk_structure_type__fence_create_info            = 8,
    vk_structure_type__semaphore_create_info        = 9,
    vk_structure_type__command_pool_create_info     = 39,
    vk_structure_type__command_buffer_allocate_info = 40,
    vk_structure_type__command_buffer_begin_info    = 42,
    vk_structure_type__swapchain_create_info        = 1000001000,
    vk_structure_type__debug_report_callback_info   = 1000011000
};

enum vk_device_type
{
    vk_device_type__other = 0,
    vk_device_type__integrated_gpu = 1,
    vk_device_type__discrete_gpu = 2,
    vk_device_type__virtual_gpu = 3,
    vk_device_type__cpu = 4,
    vk_device_type__max = 0x7FFFFFFF
};

struct vk_application_info
{
    uint32_t                                   s_type;
    uint32_t                                   reserved0;
    const void*                                p_next;
    const char*                                p_app_name;
    uint32_t                                   app_version;
    const char*                                p_engine_name;
    uint32_t                                   engine_version;
    uint32_t                                   api_version;
};

struct vk_instance_info
{
    uint32_t                                   s_type;
    uint32_t                                   reserved0;
    const void*                                p_next;
    uint32_t                                   reserved1;
    struct vk_application_info*                p_app_info;
    uint32_t                                  layer_count;
    const char* const*                         p_layer_names;
    uint32_t                                   extension_count;
    const char* const*                         p_extension_names;
};

struct vk_layer
{
    char                                       name[VK_MAX_NAME_LENGTH];
    uint32_t                                   spec_version;
    uint32_t                                   impl_version;
    char                                       desc[VK_MAX_DESC_LENGTH];
};

struct vk_extension
{
    char                                       name[VK_MAX_NAME_LENGTH];
    uint32_t                                   spec_version;
};

typedef uint32_t (*vk_pfn_debug_callback)(uint32_t flags, uint32_t object_type, uint64_t object, uint32_t location, int32_t message_code, const char* layer_prefix, const char* message, void* user_data);

enum vk_debug_callback_flags
{
    vk_debug_callback_flag__information_bit         = 0x01,
    vk_debug_callback_flag__warning_bit             = 0x02,
    vk_debug_callback_flag__performance_warning_bit = 0x04,
    vk_debug_callback_flag__error_bit               = 0x08,
    vk_debug_callback_flag__debug_bit               = 0x10,
    vk_debug_callback_flag__all_bits                = 0x1F
};

enum vk_debug_callback_object_type
{
    vk_debug_callback_object_type__unknown = 0,
    vk_debug_callback_object_type__instance = 1,
    vk_debug_callback_object_type__physical_device = 2,
    vk_debug_callback_object_type__device = 3,
    vk_debug_callback_object_type__queue = 4,
    vk_debug_callback_object_type__semaphore = 5,
    vk_debug_callback_object_type__command_buffer = 6,
    vk_debug_callback_object_type__fence = 7,
    vk_debug_callback_object_type__device_memory = 8,
    vk_debug_callback_object_type__buffer = 9,
    vk_debug_callback_object_type__image = 10,
    vk_debug_callback_object_type__event = 11,
    vk_debug_callback_object_type__query_pool = 12,
    vk_debug_callback_object_type__buffer_view = 13,
    vk_debug_callback_object_type__image_view = 14,
    vk_debug_callback_object_type__shader_module = 15,
    vk_debug_callback_object_type__pipeline_cache = 16,
    vk_debug_callback_object_type__pipeline_layout = 17,
    vk_debug_callback_object_type__render_pass = 18,
    vk_debug_callback_object_type__pipeline = 19,
    vk_debug_callback_object_type__descriptor_set_layout = 20,
    vk_debug_callback_object_type__sampler = 21,
    vk_debug_callback_object_type__descriptor_pool = 22,
    vk_debug_callback_object_type__descriptor_set = 23,
    vk_debug_callback_object_type__framebuffer = 24,
    vk_debug_callback_object_type__command_pool = 25,
    vk_debug_callback_object_type__surface = 26,
    vk_debug_callback_object_type__swapchain = 27,
    vk_debug_callback_object_type__debug_report = 28,
    vk_debug_callback_object_type__display = 29,
    vk_debug_callback_object_type__display_mode = 30,
    vk_debug_callback_object_type__validation_cache = 33,
    vk_debug_callback_object_type__sampler_ycbcr_conversion = 1000156000,
    vk_debug_callback_object_type__descriptor_update_template = 1000085000,
    vk_debug_callback_object_type__max = 0x7FFFFFFF
};

struct vk_debug_callback_info
{
    uint32_t                                   s_type;
    const void*                                p_next;
    uint32_t                                   flags;
    vk_pfn_debug_callback                      pfn_callback;
    void*                                      user_data;
};

struct vk_device_limits
{
    uint32_t                                   max_image_dimension_1D;
    uint32_t                                   max_image_dimension_2D;
    uint32_t                                   max_image_dimension_3D;
    uint32_t                                   max_image_dimension_cube;
    uint32_t                                   max_image_array_layers;
    uint32_t                                   max_texel_buffer_elements;
    uint32_t                                   max_uniform_buffer_range;
    uint32_t                                   max_storage_buffer_range;
    uint32_t                                   max_push_constants_size;
    uint32_t                                   max_memory_allocation_count;
    uint32_t                                   max_sampler_allocation_count;
    uint64_t                                   buffer_image_granularity;
    uint64_t                                   sparse_address_space_size;
    uint32_t                                   max_bound_descriptor_sets;
    uint32_t                                   max_per_stage_descriptor_samplers;
    uint32_t                                   max_per_stage_descriptor_uniform_buffers;
    uint32_t                                   max_per_stage_descriptor_storage_buffers;
    uint32_t                                   max_per_stage_descriptor_sampled_images;
    uint32_t                                   max_per_stage_descriptor_storage_images;
    uint32_t                                   max_per_stage_descriptor_input_attachments;
    uint32_t                                   max_per_stage_resources;
    uint32_t                                   max_descriptor_set_samplers;
    uint32_t                                   max_descriptor_set_uniform_buffers;
    uint32_t                                   max_descriptor_set_uniform_buffers_dynamic;
    uint32_t                                   max_descriptor_set_storage_buffers;
    uint32_t                                   max_descriptor_set_storage_buffers_dynamic;
    uint32_t                                   max_descriptor_set_sampled_images;
    uint32_t                                   max_descriptor_set_storage_images;
    uint32_t                                   max_descriptor_set_input_attachments;
    uint32_t                                   max_vertex_input_attributes;
    uint32_t                                   max_vertex_input_bindings;
    uint32_t                                   max_vertex_input_attribute_offset;
    uint32_t                                   max_vertex_input_binding_stride;
    uint32_t                                   max_vertex_output_components;
    uint32_t                                   max_tessellation_generation_level;
    uint32_t                                   max_tessellation_patch_size;
    uint32_t                                   max_tessellation_control_per_vertex_input_components;
    uint32_t                                   max_tessellation_control_per_vertex_output_components;
    uint32_t                                   max_tessellation_control_per_patch_output_components;
    uint32_t                                   max_tessellation_control_total_output_components;
    uint32_t                                   max_tessellation_evaluation_input_components;
    uint32_t                                   max_tessellation_evaluation_output_components;
    uint32_t                                   max_geometry_shader_invocations;
    uint32_t                                   max_geometry_input_components;
    uint32_t                                   max_geometry_output_components;
    uint32_t                                   max_geometry_output_vertices;
    uint32_t                                   max_geometry_total_output_components;
    uint32_t                                   max_fragment_input_components;
    uint32_t                                   max_fragment_output_attachments;
    uint32_t                                   max_fragment_dual_src_attachments;
    uint32_t                                   max_fragment_combined_output_resources;
    uint32_t                                   max_compute_shared_memory_size;
    uint32_t                                   max_compute_work_group_count[3];
    uint32_t                                   max_compute_work_group_invocations;
    uint32_t                                   max_compute_work_group_size[3];
    uint32_t                                   sub_pixel_precision_bits;
    uint32_t                                   sub_texel_precision_bits;
    uint32_t                                   mipmap_precision_bits;
    uint32_t                                   max_draw_indexed_index_value;
    uint32_t                                   max_draw_indirect_count;
    float                                      max_sampler_lod_bias;
    float                                      max_sampler_anisotropy;
    uint32_t                                   max_viewports;
    uint32_t                                   max_viewport_dimensions[2];
    float                                      viewport_bounds_range[2];
    uint32_t                                   viewport_sub_pixel_bits;
    size_t                                     min_memory_map_alignment;
    uint64_t                                   min_texel_buffer_offset_alignment;
    uint64_t                                   min_uniform_buffer_offset_alignment;
    uint64_t                                   min_storage_buffer_offset_alignment;
    int32_t                                    min_texel_offset;
    uint32_t                                   max_texel_offset;
    int32_t                                    min_texel_gather_offset;
    uint32_t                                   max_texel_gather_offset;
    float                                      min_interpolation_offset;
    float                                      max_interpolation_offset;
    uint32_t                                   sub_pixel_interpolation_offset_bits;
    uint32_t                                   max_framebuffer_width;
    uint32_t                                   max_framebuffer_height;
    uint32_t                                   max_framebuffer_layers;
    uint32_t                                   framebuffer_color_sample_counts;
    uint32_t                                   framebuffer_depth_sample_counts;
    uint32_t                                   framebuffer_stencil_sample_counts;
    uint32_t                                   framebuffer_no_attachments_sample_counts;
    uint32_t                                   max_color_attachments;
    uint32_t                                   sampled_image_color_sample_counts;
    uint32_t                                   sampled_image_integer_sample_counts;
    uint32_t                                   sampled_image_depth_sample_counts;
    uint32_t                                   sampled_image_stencil_sample_counts;
    uint32_t                                   storage_image_sample_counts;
    uint32_t                                   max_sample_mask_words;
    uint32_t                                   timestamp_compute_and_graphics;
    float                                      timestamp_period;
    uint32_t                                   max_clip_distances;
    uint32_t                                   max_cull_distances;
    uint32_t                                   max_combined_clip_and_cull_distances;
    uint32_t                                   discrete_queue_priorities;
    float                                      point_size_range[2];
    float                                      line_width_range[2];
    float                                      point_size_granularity;
    float                                      line_width_granularity;
    uint32_t                                   strict_lines;
    uint32_t                                   standard_sample_locations;
    uint64_t                                   optimal_buffer_copy_offset_alignment;
    uint64_t                                   optimal_buffer_copy_row_pitch_alignment;
    uint64_t                                   non_coherent_atom_size;
};

struct vk_device_sparse_properties
{
    uint32_t                                   residency_standard_2D_block_shape;
    uint32_t                                   residency_standard_2D_multisample_block_shape;
    uint32_t                                   residency_standard_3D_block_shape;
    uint32_t                                   residency_aligned_mip_size;
    uint32_t                                   residency_non_resident_strict;
};

struct vk_physical_device_properties
{
    uint32_t                                   api_version;
    uint32_t                                   driver_version;
    uint32_t                                   vendor_id;
    uint32_t                                   device_id;
    uint32_t                                   device_type;
    char                                       device_name[VK_MAX_DEVICE_NAME_LENGTH];
    uint8_t                                    pipeline_cache_uuid[VK_UUID_SIZE];
    struct vk_device_limits                    limits;
    struct vk_device_sparse_properties         sparse_properties;
};

struct vk_queue_group_properties
{
    union
    {
        struct
        {
            uint32_t graphics                  : 1;
            uint32_t compute                   : 1;
            uint32_t transfer                  : 1;
            uint32_t sparse_binding            : 1;
            uint32_t protected_memory          : 1;
            uint32_t video_decode              : 1;
            uint32_t video_encode              : 1;
            uint32_t optical_flow              : 1;
            uint32_t reserved                  : 24;
        };
        uint32_t                               all_bits;
    } queue_flags;
    uint32_t                                   queue_count;
    uint32_t                                   timestamp_valid_bits;
    struct
    {
        uint32_t                               width;
        uint32_t                               height;
        uint32_t                               depth;
    } min_image_transfer_granularity;
};

struct vk_physical_device_features
{
    uint32_t                                   robust_buffer_access;
    uint32_t                                   full_draw_index_uint32;
    uint32_t                                   image_cube_array;
    uint32_t                                   independent_blend;
    uint32_t                                   geometry_shader;
    uint32_t                                   tessellation_shader;
    uint32_t                                   sample_rate_shading;
    uint32_t                                   dual_src_blend;
    uint32_t                                   logic_op;
    uint32_t                                   multi_draw_indirect;
    uint32_t                                   draw_indirect_first_instance;
    uint32_t                                   depth_clamp;
    uint32_t                                   depth_bias_clamp;
    uint32_t                                   fill_mode_non_solid;
    uint32_t                                   depth_bounds;
    uint32_t                                   wide_lines;
    uint32_t                                   large_points;
    uint32_t                                   alpha_to_one;
    uint32_t                                   multi_viewport;
    uint32_t                                   sampler_anisotropy;
    uint32_t                                   texture_compression_etc2;
    uint32_t                                   texture_compression_astc_ldr;
    uint32_t                                   texture_compression_bc;
    uint32_t                                   occlusion_query_precise;
    uint32_t                                   pipeline_statistics_query;
    uint32_t                                   vertex_pipeline_stores_and_atomics;
    uint32_t                                   fragment_stores_and_atomics;
    uint32_t                                   shader_tessellation_and_geometry_point_size;
    uint32_t                                   shader_image_gather_extended;
    uint32_t                                   shader_storage_image_extended_formats;
    uint32_t                                   shader_storage_image_multisample;
    uint32_t                                   shader_storage_image_read_without_format;
    uint32_t                                   shader_storage_image_write_without_format;
    uint32_t                                   shader_uniform_buffer_array_dynamic_indexing;
    uint32_t                                   shader_sampled_image_array_dynamic_indexing;
    uint32_t                                   shader_storage_buffer_array_dynamic_indexing;
    uint32_t                                   shader_storage_image_array_dynamic_indexing;
    uint32_t                                   shader_clip_distance;
    uint32_t                                   shader_cull_distance;
    uint32_t                                   shader_float64;
    uint32_t                                   shader_int64;
    uint32_t                                   shader_int16;
    uint32_t                                   shader_resource_residency;
    uint32_t                                   shader_resource_min_lod;
    uint32_t                                   sparse_binding;
    uint32_t                                   sparse_residency_buffer;
    uint32_t                                   sparse_residency_image2D;
    uint32_t                                   sparse_residency_image3D;
    uint32_t                                   sparse_residency2_samples;
    uint32_t                                   sparse_residency4_samples;
    uint32_t                                   sparse_residency8_samples;
    uint32_t                                   sparse_residency16_samples;
    uint32_t                                   sparse_residency_aliased;
    uint32_t                                   variable_multisample_rate;
    uint32_t                                   inherited_queries;
};

struct vk_queue_create_params
{
    uint32_t                                   s_type;
    const void*                                p_next;
    uint32_t                                   flags;
    uint32_t                                   queue_group_index;
    uint32_t                                   queue_count;
    const float*                               p_queue_priorities;
};

struct vk_device_create_params
{
    uint32_t                                   s_type;
    const void*                                p_next;
    uint32_t                                   flags;
    uint32_t                                   queue_info_count;
    const struct vk_queue_create_params*       p_queue_info_array;
    uint32_t                                   layer_count; // deprecated and ignored
    const char* const*                         p_layers; // deprecated and ignored
    uint32_t                                   extension_count;
    const char* const*                         p_extensions;
    struct vk_device_features*                 features;
};

struct vk_semaphore_create_params
{
    uint32_t                                   s_type;
    const void*                                p_next;
    uint32_t                                   flags;
};

enum vk_transform
{
    vk_transform__identity = 1
};

enum vk_composite_alpha_flags
{
    vk_composite_alpha_flag__opaque = 0x1,
    vk_composite_alpha_flag__pre_multiplied = 0x2,
    vk_composite_alpha_flag__post_multiplied = 0x4,
    vk_composite_alpha_flag__inherit = 0x8
};

struct vk_surface_capabilities
{
    uint32_t                                   min_image_count;
    uint32_t                                   max_image_count;

    struct
    {
        uint32_t                               width;
        uint32_t                               height;
    } current_extent;

    struct
    {
        uint32_t                               width;
        uint32_t                               height;
    } min_image_extent;

    struct
    {
        uint32_t                               width;
        uint32_t                               height;
    } max_image_extent;

    uint32_t                                   max_image_array_layers;
    uint32_t                                   supported_transforms;
    uint32_t                                   current_transform;
    uint32_t                                   supported_composite_alpha;
    uint32_t                                   supported_usage_flags;
};

enum vk_present_mode
{
    vk_present_mode__immediate                 = 0,
    vk_present_mode__mailbox                   = 1,
    vk_present_mode__fifo                      = 2,
    vk_present_mode__fifo_relaxed              = 3,
    vk_present_mode__shared_demand_refresh     = 1000111000,
    vk_present_mode__shared_continuous_refresh = 1000111001
};

struct vk_surface_format
{
    uint32_t                                   format;
    uint32_t                                   colorspace;
};

enum vk_format
{
    vk_format__unorm_r8g8b8a8 = 37 // unsigned normalized 8-bit rgba packed into a uint32_t
};

enum vk_sharing_mode
{
    vk_sharing_mode__exclusive  = 0,
    vk_sharing_mode__concurrent = 1
};

struct vk_swapchain_create_params
{
    uint32_t                                   s_type;
    const void*                                p_next;
    uint32_t                                   flags;
    vk_surface                                 surface;
    uint32_t                                   minimum_image_count;
    uint32_t                                   surface_format;
    uint32_t                                   surface_colorspace;
    struct
    {
        uint32_t                               width;
        uint32_t                               height;
    } surface_extent;
    uint32_t                                   image_array_layers;
    uint32_t                                   image_usage_flags;
    uint32_t                                   image_sharing_mode;
    uint32_t                                   queue_family_index_count;
    const uint32_t*                            queue_family_index_array;
    uint32_t                                   pre_transform;
    uint32_t                                   composite_alpha;
    uint32_t                                   present_mode;
    uint32_t                                   clipped;
    vk_swapchain                               old_swapchain;
};

enum vk_command_pool_flags
{
    vk_command_pool_flag__transient_bit = 0x1,
    vk_command_pool_flag__reset_bit     = 0x2,
    vk_command_pool_flag__protected_bit = 0x4
};

struct vk_command_pool_create_params
{
    uint32_t                                   s_type;
    const void*                                p_next;
    uint32_t                                   flags;
    uint32_t                                   queue_family_index;
};

enum command_buffer_level
{
    command_buffer_level__primary   = 0,
    command_buffer_level__secondary = 1
};

struct vk_command_buffer_allocate_params
{
    uint32_t                                   s_type;
    const void*                                p_next;
    vk_command_pool                            command_pool;
    uint32_t                                   level;
    uint32_t                                   count;
};

enum vk_command_buffer_usage_flags
{
    vk_command_buffer_usage_flag__one_time_submit      = 0x1,
    vk_command_buffer_usage_flag__render_pass_continue = 0x2,
    vk_command_buffer_usage_flag__simultaneous_use     = 0x4
};

enum vk_query_control_flags
{
    vk_query_control_flag__precise = 0x1
};

enum vk_query_pipeline_statitic_flags
{
    vk_query_pipeline_statitic_flag__input_assembly_vertices = 0x00000001,
    vk_query_pipeline_statitic_flag__input_assembly_primitives = 0x00000002,
    vk_query_pipeline_statitic_flag__vertex_shader_invocations = 0x00000004,
    vk_query_pipeline_statitic_flag__geometry_shader_invocations = 0x00000008,
    vk_query_pipeline_statitic_flag__geometry_shader_primitives = 0x00000010,
    vk_query_pipeline_statitic_flag__clipping_invocations = 0x00000020,
    vk_query_pipeline_statitic_flag__clipping_primitives = 0x00000040,
    vk_query_pipeline_statitic_flag__fragment_shader_invocations = 0x00000080,
    vk_query_pipeline_statitic_flag__tessellation_control_shader_patches = 0x00000100,
    vk_query_pipeline_statitic_flag__evaluation_shader_invocations = 0x00000200,
    vk_query_pipeline_statitic_flag__compute_shader_invocations = 0x00000400,
    vk_query_pipeline_statitic_flag__task_shader_invocations = 0x00000800,
    vk_query_pipeline_statitic_flag__mesh_shader_invocations = 0x00001000,
    vk_query_pipeline_statitic_flag__cluster_culling_shader_invocations = 0x00002000,
    vk_query_pipeline_statitic_flag__max = 0x7FFFFFFF
};

struct vk_command_buffer_inheritance_info
{
    uint32_t                                   s_type;
    const void*                                p_next;
    vk_render_pass                             render_pass;
    uint32_t                                   subpass;
    vk_framebuffer                             framebuffer;
    uint32_t                                   occlusion_query_enable;
    uint32_t                                   query_flags;
    uint32_t                                   pipeline_statistics;
};

struct vk_command_buffer_begin_params
{
    uint32_t                                   s_type;
    const void*                                p_next;
    uint32_t                                   usage_flags;
    struct vk_command_buffer_inheritance_info* p_inheritance_info;
};

typedef void*    (*pfn_vk_get_instance_proc_addr)(vk_instance h_instance, const char* p_name);
typedef void*    (*pfn_vk_get_device_proc_addr)(vk_device h_device, const char* p_name);

// global functions
typedef uint32_t (*pfn_vk_create_instance)(struct vk_instance_info* p_info, const void* reserved, vk_instance* p_instance);
typedef void     (*pfn_vk_destroy_instance)(vk_instance h_instance, const void* reserved);
typedef uint32_t (*pfn_vk_get_version)(uint32_t* p_version);
typedef uint32_t (*pfn_vk_enumerate_layers)(uint32_t* p_count, struct vk_layer* p_layers);
typedef uint32_t (*pfn_vk_enumerate_extensions)(const char* p_layer_name, uint32_t* p_count, struct vk_extension* p_extensions);

// instance level functions
typedef uint32_t (*pfn_vk_enumerate_physical_devices)(vk_instance h_instance, uint32_t* p_count, vk_physical_device* p_devices);
typedef void     (*pfn_vk_get_physical_device_properties)(vk_physical_device h_device, struct vk_physical_device_properties* p_properties);
typedef void     (*pfn_vk_get_physical_device_features)(vk_physical_device h_device, struct vk_physical_device_features* p_features);
typedef void     (*pfn_vk_get_physical_queue_group_properties)(vk_physical_device h_device, uint32_t* p_count, struct vk_queue_group_properties* p_properties);
typedef uint32_t (*pfn_vk_create_device)(vk_physical_device h_physical_device, const struct vk_device_create_params* params, const void* reserved, vk_device* p_device);
typedef uint32_t (*pfn_vk_register_debug_callback)(vk_instance h_instance, const struct vk_debug_callback_info* p_info, const void* reserved, vk_debug_callback* p_callback);
typedef void     (*pfn_vk_unregister_debug_callback)(vk_instance instance, vk_debug_callback* callback, const void* reserved);
typedef uint32_t (*pfn_vk_get_physical_device_surface_support)(vk_physical_device h_device, uint32_t queue_family, vk_surface surface, uint32_t* p_supported);
typedef uint32_t (*pfn_vk_enumerate_device_layers)(vk_physical_device h_device, uint32_t* p_count, struct vk_layer* p_layers);
typedef uint32_t (*pfn_vk_enumerate_device_extensions)(vk_physical_device h_device, const char* p_layer_name, uint32_t* p_count, struct vk_extension* p_extensions);
typedef uint32_t (*pfn_vk_get_physical_device_surface_capabilities)(vk_physical_device h_device, vk_surface surface, struct vk_surface_capabilities* p_capabilities);
typedef uint32_t (*pfn_vk_get_physical_device_surface_present_modes)(vk_physical_device h_device, vk_surface surface, uint32_t* p_present_mode_count, uint32_t* p_present_mode_array);
typedef uint32_t (*pfn_vk_get_physical_device_surface_formats)(vk_physical_device h_device, vk_surface surface, uint32_t* p_format_count, struct vk_surface_format* p_format_array);

// device level functions
typedef uint32_t (*pfn_vk_create_semaphore)(vk_device h_device, const struct vk_semaphore_create_params* params, const void* reserved, vk_semaphore* p_semaphore);
typedef void     (*pfn_vk_destroy_semaphore)(vk_device h_device, vk_semaphore semaphore, const void* reserved);
typedef uint32_t (*pfn_vk_create_swapchain)(vk_device h_device, const struct vk_swapchain_create_params* params, const void* reserved, vk_swapchain* p_swapchain);
typedef void     (*pfn_vk_destroy_swapchain)(vk_device h_device, vk_swapchain swapchain, const void* reserved);
typedef uint32_t (*pfn_vk_acquire_next_image)(vk_device h_device, vk_swapchain swapchain, uint64_t timeout, vk_semaphore semaphore, vk_fence fence, uint32_t* p_index);
typedef uint32_t (*pfn_vk_create_command_pool)(vk_device h_device, const struct vk_command_pool_create_params* params, const void* reserved, vk_command_pool* p_command_pool);
typedef void     (*pfn_vk_destroy_command_pool)(vk_device h_device, vk_command_pool command_pool, const void* reserved);
typedef uint32_t (*pfn_vk_allocate_command_buffers)(vk_device h_device, const struct vk_command_buffer_allocate_params* params, vk_command_buffer* p_command_buffers);
typedef void     (*pfn_vk_free_command_buffers)(vk_device h_device, vk_command_pool command_pool, uint32_t command_buffer_count, const vk_command_buffer* p_command_buffers);
typedef uint32_t (*pfn_vk_wait_for_device_idle)(vk_device h_device);
typedef void     (*pfn_vk_destroy_device)(vk_device h_device, const void* reserved);
typedef uint32_t (*pfn_vk_begin_command_buffer)(vk_command_buffer command_buffer, const struct vk_command_buffer_begin_params* params);

#endif // VK_H
