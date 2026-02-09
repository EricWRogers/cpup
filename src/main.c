#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_iostream.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/types.h"
#include "cpup/opengl.h"
#include "cpup/window.h"

AppContext appContext;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
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
    
    SDL_IOStream* file = SDL_IOFromFile("assets/textures/canis_engine_icon.tga", "r");
    u8* imageData = NULL;
    u32 texture = 0;

    if (file)
    {
        printf("File found! %p\n", file);

        u8 idLength = 0;
        u8 colorMap = 0;
        u8 imageType = 0;

        // color map specification
        u16 entryIndex = 0;
        u16 mapLength = 0;
        u8 mapEntrySize = 0;

        // image specification
        u16 xOrigin = 0;
        u16 yOrigin = 0;
        u16 width = 0;
        u16 height = 0;
        u8  pixelDepth = 0; // bits per pixel
        u8  imageDescriptor = 0;
        // image descriptor:
        // bit 3-0 give the alpha channel depth
        // bit 4 of the image descriptor byte indicates right-to-left pixel ordering if set.
        // bit 5 indicates an ordering of top-to-bottom. Otherwise, pixels are stored in bottom-to-top, left-to-right order.
        // bit 7-6 of the image descriptor byte indicate the data storage interleaving mode.

        // todo read the return value to check for error
        SDL_ReadIO(file, &idLength, sizeof(u8));
        SDL_ReadIO(file, &colorMap, sizeof(u8));
        SDL_ReadIO(file, &imageType, sizeof(u8));
        SDL_ReadIO(file, &entryIndex, sizeof(u16));
        SDL_ReadIO(file, &mapLength, sizeof(u16));
        SDL_ReadIO(file, &mapEntrySize, sizeof(u8));
        SDL_ReadIO(file, &xOrigin, sizeof(u16));
        SDL_ReadIO(file, &yOrigin, sizeof(u16));
        SDL_ReadIO(file, &width, sizeof(u16));
        SDL_ReadIO(file, &height, sizeof(u16));
        SDL_ReadIO(file, &pixelDepth, sizeof(u8));
        SDL_ReadIO(file, &imageDescriptor, sizeof(u8));
        

        printf("idLength %x\n", idLength);
        printf("colorMap %x\n", colorMap);
        printf("imageType %x\n", imageType);
        printf("entryIndex %hu\n", entryIndex);
        printf("mapLength %hu\n", mapLength);
        printf("mapEntrySize %x\n", mapEntrySize);
        printf("xOrigin %hu\n", xOrigin);
        printf("yOrigin %hu\n", yOrigin);
        printf("width %hu\n", width);
        printf("height %hu\n", height);
        //printf("pixelDepth %x\n", pixelDepth);
        printf("pixelDepth %u (0x%02X)\n", (unsigned)pixelDepth, (unsigned)pixelDepth);

        if (imageDescriptor & BIT(0))
            printf("imageDescriptor bit 0\n");
        if (imageDescriptor & BIT(1))
            printf("imageDescriptor bit 1\n");
        if (imageDescriptor & BIT(2))
            printf("imageDescriptor bit 2\n");
        if (imageDescriptor & BIT(3))
            printf("imageDescriptor bit 3\n");
        if (imageDescriptor & BIT(4))
            printf("imageDescriptor bit 4\n");
        if (imageDescriptor & BIT(5))
            printf("imageDescriptor bit 5\n");
        if (imageDescriptor & BIT(6))
            printf("imageDescriptor bit 6\n");
        if (imageDescriptor & BIT(7))
            printf("imageDescriptor bit 7\n");
        
        u32 bytesPerPixel = pixelDepth / 8;
        size_t srcSize = (size_t)width * (size_t)height * bytesPerPixel;

        u8* srcBGRA = (u8*)malloc(srcSize);
        SDL_ReadIO(file, srcBGRA, srcSize); // data is bgra

        bool originTop = (imageDescriptor & BIT(5));

        u8* p = NULL;

        u8 b = 0;
        u8 g = 0;
        u8 r = 0;
        u8 a = 0;

        const size_t dstSize = (size_t)width * (size_t)height;
        for (u32 i = 0; i < dstSize; i++)
        {
            p = srcBGRA + i * 4;

            b = p[0];
            g = p[1];
            r = p[2];
            a = p[3];

            p[0] = r;
            p[1] = g;
            p[2] = b;
            p[3] = a;
        }
        
        /*const size_t dstSize = (size_t)width * (size_t)height * 4;
        u8* data = (u8*)malloc(dstSize);
        if (!data) {
            free(srcBGRA);
            return false;
        }
        
        for (u32 y = 0; y < height; ++y) {
            u32 srcY = originTop ? y : (height - 1 - y);

            const u8* srcRow = srcBGRA + (size_t)srcY * (size_t)width * 4;
            u8* dstRow = data + (size_t)y * (size_t)width * 4;

            for (u32 x = 0; x < width; ++x) {
                const u8* p = srcRow + x * 4;

                u8 b = p[0];
                u8 g = p[1];
                u8 r = p[2];
                u8 a = p[3];

                dstRow[x*4 + 0] = r;
                dstRow[x*4 + 1] = g;
                dstRow[x*4 + 2] = b;
                dstRow[x*4 + 3] = a;
            }
        }*/

        //free(srcBGRA);

        imageData = srcBGRA;

        SDL_CloseIO(file);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //////////

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    
    bool running = true;
    while(running) {
        // imput
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "mainTexture"), 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        window_swap(&appContext);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    window_destroy(&appContext);
    return 0;
}
