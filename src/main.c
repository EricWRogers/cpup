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
#include "cpup/opengl.h"
#include "cpup/window.h"

AppContext appContext;

int main(int argc, char *argv[])
{
    if (window_init(&appContext) > 0)
        return 1;
    
    bool running = true;
    while(running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
    }

    window_destroy(&appContext);
    return 0;
}
