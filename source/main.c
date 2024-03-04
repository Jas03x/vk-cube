
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "vk_context.h"

const char* window_title = "vk-cube";
const uint32_t window_width = 1024;
const uint32_t window_height = 768;

SDL_Window* g_window = NULL;

bool initialize(void)
{
    bool status = true;

    uint32_t ext_count = 0;
    const char** ext_array = NULL;

    vk_surface surface = NULL;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Could not initialize SDL\n");
        status = false;
    }

    if(status)
    {
        if(SDL_Vulkan_LoadLibrary(NULL) != 0)
        {
            printf("Could not load the vulkan library\n");
            status = false;
        }
    }

    if(status)
    {
        g_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

        if (g_window == NULL)
        {
            status = false;
            printf("Could not create SDL window\n");
        }
    }

    if(status)
    {
        if(SDL_Vulkan_GetInstanceExtensions(g_window, &ext_count, NULL) != SDL_TRUE)
        {
            status = false;
            printf("Could not get required SDL extension count\n");
        }
    }

    if(status)
    {
        ext_array = malloc(ext_count * sizeof(char*));

        if(ext_array == NULL)
        {
            status = false;
            printf("Failed to allocate memory\n");
        }
    }

    if(status)
    {
        if(SDL_Vulkan_GetInstanceExtensions(g_window, &ext_count, ext_array) != SDL_TRUE)
        {
            status = false;
            printf("Could not get required SDL extension count\n");
        }
    }

    if(status && (ext_count > 0))
    {
        printf("Required SDL extensions:\n");
        for(uint32_t i = 0; i < ext_count; i++)
        {
            printf("\t%s\n", ext_array[i]);
        }
    }

    if(status)
    {
        status = initialize_vulkan_context(SDL_Vulkan_GetVkGetInstanceProcAddr(), ext_count, ext_array);
    }

    if(status)
    {
        if(SDL_Vulkan_CreateSurface(g_window, vk_ctx->instance, (VkSurfaceKHR*) &surface) != SDL_TRUE)
        {
            printf("Could not create vulkan surface\n");
            status = false;
        }
    }

    if(status)
    {
        status = initialize_swapchain(surface);
    }

    if(status)
    {
        status = initialize_queues();
    }

    if(ext_array != NULL)
    {
        free(ext_array);
        ext_array = NULL;
    }

    return status;
}

void uninitialize(void)
{
    uninitialize_vulkan_context();

    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();
}

bool render(void)
{
    bool status = true;

    struct vk_command_buffer_begin_params params;
    params.s_type = vk_structure_type__command_buffer_begin_info;
    params.p_next = NULL;
    params.usage_flags = vk_command_buffer_usage_flag__one_time_submit;
    params.p_inheritance_info = NULL;

    if(vk_ctx->begin_command_buffer(vk_ctx->command_buffer, &params) != vk_success)
    {
        printf("Failed to begin command buffer\n");
        status = false;
    }

    return status;
}

bool run(void)
{
    bool status = true;
    bool exit = false;
    SDL_Event event = { 0 };

    while(status)
    {
        while(status && (SDL_PollEvent(&event) != 0))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    exit = true;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        if(!exit)
        {
            status = render();

            if(status)
            {
                status = present();
            }
        }
        else
        {
            break;
        }
    }

    return status;
}

int main(int argc, char* argv[])
{
    int status = 0;

    if(!initialize())
    {
        status = -1;
    }

    if(status == 0)
    {
        if(!run())
        {
            status = -1;
        }
    }

    uninitialize();

    printf("Exit with code 0x%X\n", status);

    return status;
}
