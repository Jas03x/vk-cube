
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "vk.h"

const char* window_title = "vk-cube";
const uint32_t window_width = 1024;
const uint32_t window_height = 768;

SDL_Window* g_window = NULL;

struct vk_functions
{
    pfn_vk_get_instance_proc_addr get_instance_proc_addr;
    pfn_vk_create_instance        create_instance;
};

struct vk_functions vk_api = { 0 };

bool initialize(void)
{
    bool status = true;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        status = false;
        printf("error: could not initialize sdl\n");
    }

    if(status)
    {
        SDL_Vulkan_LoadLibrary(NULL);
    }

    if(status)
    {
        g_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

        if (g_window == NULL)
        {
            status = false;
            printf("error: could not create sdl window\n");
        }
    }

    vk_api.get_instance_proc_addr = SDL_Vulkan_GetVkGetInstanceProcAddr();
    vk_api.create_instance = vk_api.get_instance_proc_addr(NULL, "vkCreateInstance");

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

    vk_instance instance = NULL;

    vk_api.create_instance(&instance_info, NULL, &instance);

    printf("instance: %p\n", instance);

    return status;
}

void uninitialize(void)
{
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();
}

bool render(void)
{
    bool status = true;

    return status;
}

bool run(void)
{
    bool status = true;
    bool exit = false;
    SDL_Event event = { 0 };

    while(status)
    {
        while(SDL_PollEvent(&event) != 0)
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
            render();
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

    if(status != 0)
    {
        printf("exiting with error code 0x%X\n", status);
    }

    return status;
}

