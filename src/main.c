#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/types.h"
#include "cpup/model.h"
#include "cpup/shader.h"
#include "cpup/window.h"

AppContext appContext;

int main(int argc, char *argv[])
{
    if (InitCanis() > 0)
        return 1;
    
    if (InitWindow(&appContext) > 0)
        return 1;
    
    Image image = LoadImage("assets/textures/canis_engine_icon.tga");
    
    // build and compile our shader program
    u32 shaderProgram = GenerateShaderFromFiles("assets/shaders/logo.vs", "assets/shaders/logo.fs");
    printf("shaderID: %i\n", shaderProgram);

    float ve[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int in[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    f32* vertices = vec_init(32, sizeof(f32));
    vec_append(&vertices, ve, 32);

    u32* indices = vec_init(6, sizeof(u32));
    vec_append(&indices, in, 6);
    
    Model model = BuildModel(&vertices, &indices, STATIC_DRAW);
    
    bool running = true;
    f32 time = 0.0f;
    while(running) {
        // imput
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_KEY_UP)
            {
                if (event.key.scancode == SDL_SCANCODE_R)
                {
                    printf("Load new shader!\n");
                    u32 newShader = GenerateShaderFromFiles("assets/shaders/logo.vs", "assets/shaders/logo.fs");

                    if (newShader != 0)
                    {
                        DeleteShader(shaderProgram);
                        shaderProgram = newShader;
                    }
                }
            }
        }

        // render
        ClearWindow();

        // draw our first triangle
        // bind the shader
        BindShader(shaderProgram);
        ShaderBindTexture(shaderProgram, image.id, "MAIN_TEXTURE", 0);
        ShaderSetFloat(shaderProgram, "TIME", SDL_GetTicks()/1000.0f);
        
        DrawModel(model);
        DrawModel(model);
        UnBindShader();

        SwapWindow(&appContext);
    }

    FreeModel(model);

    free(image.data);

    FreeWindow(&appContext);

    DeleteShader(shaderProgram);
    return 0;
}
