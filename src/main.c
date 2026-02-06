#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/types.h"

typedef struct {
    void* window;
    void* glContext;
} AppContext;

AppContext appContext;

void RunWindow();

int main(int argc, char *argv[])
{
    RunWindow();
    return 0;
}

void RunWindow()
{
    #ifdef LINUX
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
    #endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_SetHint(SDL_HINT_RENDER_GPU_LOW_POWER, "0"); // prefer high perforance GPU

    appContext.window = (void*)SDL_CreateWindow("CPup", 800, 600, 0);

    if (appContext.window == NULL)
    {

    }

    appContext.glContext = (void*)SDL_GL_CreateContext((SDL_Window*)appContext.window);

    if (appContext.glContext)
    {

    }

    glewExperimental = true;
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GLEW Failed: %s", glewGetErrorString(error));
    }

    bool running = true;
    while(running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
    }


    if (appContext.glContext)
        SDL_GL_DestroyContext(appContext.glContext);
    if (appContext.window)
        SDL_DestroyWindow(appContext.window);
}
