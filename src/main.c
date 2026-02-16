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
#include "cpup/math.h"
#include "cpup/types.h"
#include "cpup/model.h"
#include "cpup/shader.h"
#include "cpup/window.h"

AppContext app;

int main(int argc, char *argv[])
{
    if (InitCanis() > 0)
        return 1;

    app.windowWidth = 600;
    app.windowHeight = 600;
    
    if (InitWindow(&app) > 0)
        return 1;
    
    Image iconImage = LoadImage("assets/textures/canis_engine_icon.tga");
    Image containerImage = LoadImage("assets/textures/container.tga");
    
    // build and compile our shader program
    u32 shaderProgram = GenerateShaderFromFiles("assets/shaders/logo.vs", "assets/shaders/logo.fs");
    printf("shaderID: %i\n", shaderProgram);

    float ve[] = {
        // positions            // texture coords
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f  // top left 
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

                    free(iconImage.data);
                    iconImage = LoadImage("assets/textures/canis_engine_icon.tga");
                    
                    free(containerImage.data);
                    containerImage = LoadImage("assets/textures/container.tga");
                }
            }
        }

        // render
        ClearWindow();

        Matrix4 projection = Mat4Orthographic(0.0f, (float)app.windowWidth, 0.0f, (float)app.windowHeight, 0.001f, 100.0f); 
        Matrix4 view = IdentityMatrix4(); 
        Mat4Translate(&view, InitVector3(0.0f, 0.0f, -0.5f));
        
        Matrix4 transform = IdentityMatrix4();
        Mat4Translate(&transform, InitVector3(300.0f, 300.0f, 0.0f));
        Mat4Scale(&transform, InitVector3(128.0f, 128.0f, 1.0f));

        Matrix4 transform2 = IdentityMatrix4();
        Mat4Translate(&transform2, InitVector3(450.0f, 450.0f, 0.0f));
        Mat4Scale(&transform2, InitVector3(128.0f, 128.0f, 1.0f));

        // draw our first triangle
        // bind the shader
        BindShader(shaderProgram);
        ShaderSetFloat(shaderProgram, "TIME", SDL_GetTicks()/1000.0f);
        ShaderSetMatrix4(shaderProgram, "VIEW", view);
        ShaderSetMatrix4(shaderProgram, "PROJECTION", projection);

        ShaderBindTexture(shaderProgram, iconImage.id, "MAIN_TEXTURE", 0);
        ShaderSetMatrix4(shaderProgram, "TRANSFORM", transform);
        DrawModel(model);

        ShaderBindTexture(shaderProgram, containerImage.id, "MAIN_TEXTURE", 0);
        ShaderSetMatrix4(shaderProgram, "TRANSFORM", transform2);
        DrawModel(model);
        UnBindShader();

        SwapWindow(&app);
    }

    FreeModel(model);

    free(iconImage.data);
    free(containerImage.data);

    FreeWindow(&app);

    DeleteShader(shaderProgram);
    return 0;
}
