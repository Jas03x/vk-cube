
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

    status = initialize_vulkan_context(SDL_Vulkan_GetVkGetInstanceProcAddr());

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

    printf("exit with code 0x%X\n", status);

    return status;
}
