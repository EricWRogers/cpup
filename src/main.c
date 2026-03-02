#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/ecs.h"
#include "cpup/vec.h"
#include "cpup/math.h"
#include "cpup/types.h"
#include "cpup/model.h"
#include "cpup/shader.h"
#include "cpup/window.h"

AppContext app;

typedef struct {
    Vector3 position;
    Vector3 scale;
} Transform;

typedef struct {
    Vector3 velocity;
} Rigidbody;

typedef struct {
    Vector4 color;
    u32     shader;
} Material;

u64 TRANSFORM_ID;
u64 RIGIDBODY_ID;
u64 MATERIAL_ID;

void UpdateBullets(ECS *_ecs, u32 _entityId, void *_userData) {
    Transform* transform = GetComponent(_ecs, _entityId, TRANSFORM_ID);
    Rigidbody* rigidbody = GetComponent(_ecs, _entityId, RIGIDBODY_ID);
    int* count = _userData;
    *count += 1;
    //printf("count: %i\n", *count);
}

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

    ECS ecs = InitECS(100000);

    TRANSFORM_ID = RegisterComponent(&ecs, sizeof(Transform));
    RIGIDBODY_ID = RegisterComponent(&ecs, sizeof(Rigidbody));
    MATERIAL_ID = RegisterComponent(&ecs, sizeof(Material));

    u32 entity1 = CreateEntity(&ecs);

    {
        Transform* t = AddComponent(&ecs, entity1, TRANSFORM_ID);
        Material* m = AddComponent(&ecs, entity1, MATERIAL_ID);

        m->color.x = 1.0f;
    }

    // create bullets
    for(int i = 0; i < 1000000 - 1; i++) {
        u32 entity = CreateEntity(&ecs);
        Transform* t = AddComponent(&ecs, entity, TRANSFORM_ID);
        Rigidbody* r = AddComponent(&ecs, entity, RIGIDBODY_ID);
        Material* m = AddComponent(&ecs, entity, MATERIAL_ID);
    }

    
    f32 gameTime = 0.0f;
    f32 deltaTime = 0.0f;

    

    {
        Material* m = GetComponent(&ecs, entity1, MATERIAL_ID);

        m->color.y = 2.0f;
        printf("Color: %f %f %f %f\n", m->color.x, m->color.y, m->color.z, m->color.w);
    }
    

    
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

        if (gameTime != 0.0f)
            deltaTime = (SDL_GetTicksNS() * 1e-9) -  gameTime;
        
        gameTime = SDL_GetTicksNS() * 1e-9;
        int count = 0;
        ForEachEntityWithComponents(&ecs, TRANSFORM_ID | RIGIDBODY_ID | MATERIAL_ID, UpdateBullets, &count);
        printf("FPS: %f\n", 1.0f/deltaTime);

        Matrix4 projection = Mat4Orthographic(0.0f, (float)app.windowWidth, 0.0f, (float)app.windowHeight, 0.001f, 100.0f); 
        Matrix4 view = IdentityMatrix4(); 
        Mat4Translate(&view, InitVector3(sin((double)SDL_GetTicks()) * 20.0f, 0.0f, -0.5f));
        
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
