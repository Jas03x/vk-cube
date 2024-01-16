#ifndef VK_H
#define VK_H

#define VK_MAX_EXT_NAME_LENGTH 256
#define VK_MAX_EXT_DESC_LENGTH 256

#define VK_VERSION(variant, major, minor, patch) ((((variant) & 0x3) << 29) | (((major) & 0x7F) << 22) | (((minor) & 0x3FF) << 12) | ((patch) & 0xFFF))

typedef void* vk_instance;

enum vk_result
{
    vk_success = 0
};

enum vk_structure_type
{
    vk_application_info = 0,
    vk_instance_info    = 1
};

struct vk_application_info
{
    uint32_t                    type;
    uint32_t                    reserved0;
    const void*                 next;
    const char*                 app_name;
    uint32_t                    app_version;
    const char*                 engine_name;
    uint32_t                    engine_version;
    uint32_t                    api_version;
};

struct vk_instance_info
{
    uint32_t                    type;
    uint32_t                    reserved0;
    const void*                 next;
    uint32_t                    reserved1;
    struct vk_application_info* app_info;
    uint32_t                    layer_count;
    const char* const*          layer_names;
    uint32_t                    extension_count;
    const char* const*          extension_names;
};

struct vk_layer
{
    char                        name[VK_MAX_EXT_NAME_LENGTH];
    uint32_t                    spec_version;
    uint32_t                    impl_version;
    char                        desc[VK_MAX_EXT_DESC_LENGTH];
};

typedef void*    (*pfn_vk_get_instance_proc_addr)(vk_instance instance, const char* name);
typedef uint32_t (*pfn_vk_create_instance)(struct vk_instance_info* info, void* reserved, vk_instance* instance);
typedef uint32_t (*pfn_vk_get_version)(uint32_t* p_version);
typedef uint32_t (*pfn_vk_get_layers)(uint32_t* p_count, struct vk_layer* p_layers);

#endif // VK_H
