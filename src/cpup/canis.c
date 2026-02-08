#include "canis.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>

i32 canis_init()
{
    #ifdef __linux__
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
    #endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD) == false)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    return 0;
}