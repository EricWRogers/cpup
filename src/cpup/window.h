#pragma once
#include "canis.h"
#include "opengl.h"

#include <SDL3/SDL.h>

int window_init(AppContext* _appContext)
{
    #ifdef __linux__
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
    #endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD) == false)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_SetHint(SDL_HINT_RENDER_GPU_LOW_POWER, "0"); // prefer high perforance GPU

    _appContext->window = (void*)SDL_CreateWindow("CPup", 800, 600, SDL_WINDOW_OPENGL);

    if (_appContext->window == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        return 1;
    }

    _appContext->glContext = (void*)SDL_GL_CreateContext((SDL_Window*)_appContext->window);

    if (_appContext->glContext == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create GL context: %s", SDL_GetError());
        return 1;
    }

    glewExperimental = true;
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GLEW Failed: %s", glewGetErrorString(error));
        return 1;
    }

    return 0;
}

void window_destroy(AppContext* _appContext)
{
    if (_appContext->glContext)
        SDL_GL_DestroyContext((SDL_GLContext)_appContext->glContext);
    if (_appContext->window)
        SDL_DestroyWindow((SDL_Window*)_appContext->window);
    
    _appContext->glContext = NULL;
    _appContext->window = NULL;
}