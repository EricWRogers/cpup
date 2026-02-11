#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/types.h"
#include "cpup/model.h"
#include "cpup/shader.h"
#include "cpup/window.h"

AppContext appContext;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "uniform float time;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   gl_Position.x += sin(time);\n"
    "   ourColor = aColor;\n"
	"   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D mainTexture;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(mainTexture, TexCoord);\n"
    "}\n\0";

int main(int argc, char *argv[])
{
    if (canis_init() > 0)
        return 1;
    
    if (window_init(&appContext) > 0)
        return 1;
    
    Image image = LoadImage("assets/textures/canis_engine_icon.tga");
    
    // build and compile our shader program
    u32 shaderProgram = GenerateShader(vertexShaderSource, fragmentShaderSource);

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

    f32* vertices = NULL;
    vec_init(&vertices, 32, sizeof(f32));
    for(int i = 0; i < 32; i++)
        vec_add(&vertices, ve + i);

    u32* indices = NULL;
    vec_init(&indices, 6, sizeof(u32));
    vec_add(&indices, in + 0);
    vec_add(&indices, in + 1);
    vec_add(&indices, in + 2);
    vec_add(&indices, in + 3);
    vec_add(&indices, in + 4);
    vec_add(&indices, in + 5);
    
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
        }

        // render
        window_clear();

        // draw our first triangle
        // bind the shader
        ShaderUse(shaderProgram);
        ShaderBindTexture(shaderProgram, image.id, "mainTexture", 0);
        ShaderSetFloat(shaderProgram, "time", time+=0.016f);
        
        DrawModel(model);
        DrawModel(model);

        window_swap(&appContext);
    }

    FreeModel(model);

    free(image.data);

    window_destroy(&appContext);
    return 0;
}
