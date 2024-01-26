#ifndef VK_H
#define VK_H

#define VK_MAX_NAME_LENGTH 256
#define VK_MAX_DESC_LENGTH 256
#define VK_MAX_DEVICE_NAME_LENGTH 256
#define VK_UUID_SIZE 16

#define VK_VERSION(variant, major, minor, patch) ((((variant) & 0x3) << 29) | (((major) & 0x7F) << 22) | (((minor) & 0x3FF) << 12) | ((patch) & 0xFFF))

typedef void* vk_instance;
typedef void* vk_physical_device;
typedef void* vk_device;

enum vk_result
{
    vk_success = 0
};

enum vk_structure_type
{
    vk_application_info   = 0,
    vk_instance_info      = 1,
    vk_queue_create_info  = 2,
    vk_device_create_info = 3
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
    uint32_t                             s_type;
    uint32_t                             reserved0;
    const void*                          p_next;
    const char*                          p_app_name;
    uint32_t                             app_version;
    const char*                          p_engine_name;
    uint32_t                             engine_version;
    uint32_t                             api_version;
};

struct vk_instance_info
{
    uint32_t                             s_type;
    uint32_t                             reserved0;
    const void*                          p_next;
    uint32_t                             reserved1;
    struct vk_application_info*          p_app_info;
    uint32_t                             layer_count;
    const char* const*                   p_layer_names;
    uint32_t                             extension_count;
    const char* const*                   p_extension_names;
};

struct vk_layer
{
    char                                 name[VK_MAX_NAME_LENGTH];
    uint32_t                             spec_version;
    uint32_t                             impl_version;
    char                                 desc[VK_MAX_DESC_LENGTH];
};

struct vk_extension
{
    char                                 name[VK_MAX_NAME_LENGTH];
    uint32_t                             spec_version;
};

struct vk_device_limits
{
    uint32_t                             max_image_dimension_1D;
    uint32_t                             max_image_dimension_2D;
    uint32_t                             max_image_dimension_3D;
    uint32_t                             max_image_dimension_cube;
    uint32_t                             max_image_array_layers;
    uint32_t                             max_texel_buffer_elements;
    uint32_t                             max_uniform_buffer_range;
    uint32_t                             max_storage_buffer_range;
    uint32_t                             max_push_constants_size;
    uint32_t                             max_memory_allocation_count;
    uint32_t                             max_sampler_allocation_count;
    uint64_t                             buffer_image_granularity;
    uint64_t                             sparse_address_space_size;
    uint32_t                             max_bound_descriptor_sets;
    uint32_t                             max_per_stage_descriptor_samplers;
    uint32_t                             max_per_stage_descriptor_uniform_buffers;
    uint32_t                             max_per_stage_descriptor_storage_buffers;
    uint32_t                             max_per_stage_descriptor_sampled_images;
    uint32_t                             max_per_stage_descriptor_storage_images;
    uint32_t                             max_per_stage_descriptor_input_attachments;
    uint32_t                             max_per_stage_resources;
    uint32_t                             max_descriptor_set_samplers;
    uint32_t                             max_descriptor_set_uniform_buffers;
    uint32_t                             max_descriptor_set_uniform_buffers_dynamic;
    uint32_t                             max_descriptor_set_storage_buffers;
    uint32_t                             max_descriptor_set_storage_buffers_dynamic;
    uint32_t                             max_descriptor_set_sampled_images;
    uint32_t                             max_descriptor_set_storage_images;
    uint32_t                             max_descriptor_set_input_attachments;
    uint32_t                             max_vertex_input_attributes;
    uint32_t                             max_vertex_input_bindings;
    uint32_t                             max_vertex_input_attribute_offset;
    uint32_t                             max_vertex_input_binding_stride;
    uint32_t                             max_vertex_output_components;
    uint32_t                             max_tessellation_generation_level;
    uint32_t                             max_tessellation_patch_size;
    uint32_t                             max_tessellation_control_per_vertex_input_components;
    uint32_t                             max_tessellation_control_per_vertex_output_components;
    uint32_t                             max_tessellation_control_per_patch_output_components;
    uint32_t                             max_tessellation_control_total_output_components;
    uint32_t                             max_tessellation_evaluation_input_components;
    uint32_t                             max_tessellation_evaluation_output_components;
    uint32_t                             max_geometry_shader_invocations;
    uint32_t                             max_geometry_input_components;
    uint32_t                             max_geometry_output_components;
    uint32_t                             max_geometry_output_vertices;
    uint32_t                             max_geometry_total_output_components;
    uint32_t                             max_fragment_input_components;
    uint32_t                             max_fragment_output_attachments;
    uint32_t                             max_fragment_dual_src_attachments;
    uint32_t                             max_fragment_combined_output_resources;
    uint32_t                             max_compute_shared_memory_size;
    uint32_t                             max_compute_work_group_count[3];
    uint32_t                             max_compute_work_group_invocations;
    uint32_t                             max_compute_work_group_size[3];
    uint32_t                             sub_pixel_precision_bits;
    uint32_t                             sub_texel_precision_bits;
    uint32_t                             mipmap_precision_bits;
    uint32_t                             max_draw_indexed_index_value;
    uint32_t                             max_draw_indirect_count;
    float                                max_sampler_lod_bias;
    float                                max_sampler_anisotropy;
    uint32_t                             max_viewports;
    uint32_t                             max_viewport_dimensions[2];
    float                                viewport_bounds_range[2];
    uint32_t                             viewport_sub_pixel_bits;
    size_t                               min_memory_map_alignment;
    uint64_t                             min_texel_buffer_offset_alignment;
    uint64_t                             min_uniform_buffer_offset_alignment;
    uint64_t                             min_storage_buffer_offset_alignment;
    int32_t                              min_texel_offset;
    uint32_t                             max_texel_offset;
    int32_t                              min_texel_gather_offset;
    uint32_t                             max_texel_gather_offset;
    float                                min_interpolation_offset;
    float                                max_interpolation_offset;
    uint32_t                             sub_pixel_interpolation_offset_bits;
    uint32_t                             max_framebuffer_width;
    uint32_t                             max_framebuffer_height;
    uint32_t                             max_framebuffer_layers;
    uint32_t                             framebuffer_color_sample_counts;
    uint32_t                             framebuffer_depth_sample_counts;
    uint32_t                             framebuffer_stencil_sample_counts;
    uint32_t                             framebuffer_no_attachments_sample_counts;
    uint32_t                             max_color_attachments;
    uint32_t                             sampled_image_color_sample_counts;
    uint32_t                             sampled_image_integer_sample_counts;
    uint32_t                             sampled_image_depth_sample_counts;
    uint32_t                             sampled_image_stencil_sample_counts;
    uint32_t                             storage_image_sample_counts;
    uint32_t                             max_sample_mask_words;
    uint32_t                             timestamp_compute_and_graphics;
    float                                timestamp_period;
    uint32_t                             max_clip_distances;
    uint32_t                             max_cull_distances;
    uint32_t                             max_combined_clip_and_cull_distances;
    uint32_t                             discrete_queue_priorities;
    float                                point_size_range[2];
    float                                line_width_range[2];
    float                                point_size_granularity;
    float                                line_width_granularity;
    uint32_t                             strict_lines;
    uint32_t                             standard_sample_locations;
    uint64_t                             optimal_buffer_copy_offset_alignment;
    uint64_t                             optimal_buffer_copy_row_pitch_alignment;
    uint64_t                             non_coherent_atom_size;
};

struct vk_device_sparse_properties
{
    uint32_t                             residency_standard_2D_block_shape;
    uint32_t                             residency_standard_2D_multisample_block_shape;
    uint32_t                             residency_standard_3D_block_shape;
    uint32_t                             residency_aligned_mip_size;
    uint32_t                             residency_non_resident_strict;
};

struct vk_physical_device_properties
{
    uint32_t                             api_version;
    uint32_t                             driver_version;
    uint32_t                             vendor_id;
    uint32_t                             device_id;
    uint32_t                             device_type;
    char                                 device_name[VK_MAX_DEVICE_NAME_LENGTH];
    uint8_t                              pipeline_cache_uuid[VK_UUID_SIZE];
    struct vk_device_limits              limits;
    struct vk_device_sparse_properties   sparse_properties;
};

struct vk_queue_group_properties
{
    union
    {
        struct
        {
            uint32_t graphics            : 1;
            uint32_t compute             : 1;
            uint32_t transfer            : 1;
            uint32_t sparse_binding      : 1;
            uint32_t protected_memory    : 1;
            uint32_t video_decode        : 1;
            uint32_t video_encode        : 1;
            uint32_t optical_flow        : 1;
            uint32_t reserved            : 24;
        };
        uint32_t                         all_bits;
    } queue_flags;
    uint32_t                             queue_count;
    uint32_t                             timestamp_valid_bits;
    struct
    {
        uint32_t                         width;
        uint32_t                         height;
        uint32_t                         depth;
    } min_image_transfer_granularity;
};

struct vk_device_features
{
    uint32_t                             robust_buffer_access;
    uint32_t                             fullDrawIndexUint32;
    uint32_t                             imageCubeArray;
    uint32_t                             independentBlend;
    uint32_t                             geometryShader;
    uint32_t                             tessellationShader;
    uint32_t                             sampleRateShading;
    uint32_t                             dualSrcBlend;
    uint32_t                             logicOp;
    uint32_t                             multiDrawIndirect;
    uint32_t                             drawIndirectFirstInstance;
    uint32_t                             depthClamp;
    uint32_t                             depthBiasClamp;
    uint32_t                             fillModeNonSolid;
    uint32_t                             depthBounds;
    uint32_t                             wideLines;
    uint32_t                             largePoints;
    uint32_t                             alphaToOne;
    uint32_t                             multiViewport;
    uint32_t                             samplerAnisotropy;
    uint32_t                             textureCompressionETC2;
    uint32_t                             textureCompressionASTC_LDR;
    uint32_t                             textureCompressionBC;
    uint32_t                             occlusionQueryPrecise;
    uint32_t                             pipelineStatisticsQuery;
    uint32_t                             vertexPipelineStoresAndAtomics;
    uint32_t                             fragmentStoresAndAtomics;
    uint32_t                             shaderTessellationAndGeometryPointSize;
    uint32_t                             shaderImageGatherExtended;
    uint32_t                             shaderStorageImageExtendedFormats;
    uint32_t                             shaderStorageImageMultisample;
    uint32_t                             shaderStorageImageReadWithoutFormat;
    uint32_t                             shaderStorageImageWriteWithoutFormat;
    uint32_t                             shaderUniformBufferArrayDynamicIndexing;
    uint32_t                             shaderSampledImageArrayDynamicIndexing;
    uint32_t                             shaderStorageBufferArrayDynamicIndexing;
    uint32_t                             shaderStorageImageArrayDynamicIndexing;
    uint32_t                             shaderClipDistance;
    uint32_t                             shaderCullDistance;
    uint32_t                             shaderFloat64;
    uint32_t                             shaderInt64;
    uint32_t                             shaderInt16;
    uint32_t                             shaderResourceResidency;
    uint32_t                             shaderResourceMinLod;
    uint32_t                             sparseBinding;
    uint32_t                             sparseResidencyBuffer;
    uint32_t                             sparseResidencyImage2D;
    uint32_t                             sparseResidencyImage3D;
    uint32_t                             sparseResidency2Samples;
    uint32_t                             sparseResidency4Samples;
    uint32_t                             sparseResidency8Samples;
    uint32_t                             sparseResidency16Samples;
    uint32_t                             sparseResidencyAliased;
    uint32_t                             variableMultisampleRate;
    uint32_t                             inheritedQueries;
};

struct vk_queue_creation_info
{
    uint32_t                             s_type;
    const void*                          p_next;
    uint32_t                             flags;
    uint32_t                             queue_group_index;
    uint32_t                             queue_count;
    const float*                         p_queue_priorities;
};

struct vk_device_creation_info
{
    uint32_t                             s_type;
    const void*                          p_next;
    uint32_t                             flags;
    uint32_t                             queue_info_count;
    const struct vk_queue_creation_info* p_queue_info_array;
    uint32_t                             layer_count; // deprecated and ignored
    const char* const*                   p_layers; // deprecated and ignored
    uint32_t                             extension_count;
    const char* const*                   p_extensions;
    struct vk_device_features*           features;
};

// global functions
typedef void*    (*pfn_vk_get_instance_proc_addr)(vk_instance h_instance, const char* p_name);
typedef uint32_t (*pfn_vk_create_instance)(struct vk_instance_info* p_info, void* p_reserved, vk_instance* p_instance);
typedef uint32_t (*pfn_vk_get_version)(uint32_t* p_version);
typedef uint32_t (*pfn_vk_enumerate_layers)(uint32_t* p_count, struct vk_layer* p_layers);
typedef uint32_t (*pfn_vk_enumerate_extensions)(const char* p_layer_name, uint32_t* p_count, struct vk_extension* p_extensions);

// instance level functions
typedef uint32_t (*pfn_vk_enumerate_physical_devices)(vk_instance h_instance, uint32_t* p_count, vk_physical_device* p_devices);
typedef void     (*pfn_vk_get_physical_device_properties)(vk_physical_device h_device, struct vk_physical_device_properties* p_properties);
typedef void     (*pfn_vk_get_physical_queue_group_properties)(vk_physical_device h_device, uint32_t* p_count, struct vk_queue_group_properties* p_properties);
typedef uint32_t (*pfn_vk_create_device)(vk_physical_device h_physical_device, struct vk_device_creation_info* p_info, void* p_reserved, vk_device* p_device);

#endif // VK_H
