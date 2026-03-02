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
    Model* model;
} Mesh;

typedef struct {
    Vector4 color;
    u32     shader;
    Image*  image;
} Material;

u64 TRANSFORM_ID;
u64 RIGIDBODY_ID;
u64 MESH_ID;
u64 MATERIAL_ID;

void UpdateBullets(ECS *_ecs, u32 _entityId, void *_userData) {
    Transform* transform = GetComponent(_ecs, _entityId, TRANSFORM_ID);
    Rigidbody* rigidbody = GetComponent(_ecs, _entityId, RIGIDBODY_ID);
    int* count = _userData;
    *count += 1;
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
    MESH_ID = RegisterComponent(&ecs, sizeof(Mesh));
    MATERIAL_ID = RegisterComponent(&ecs, sizeof(Material));

    u32 entity1 = CreateEntity(&ecs);

    {
        Transform* t = AddComponent(&ecs, entity1, TRANSFORM_ID);
        Material* m = AddComponent(&ecs, entity1, MATERIAL_ID);

        m->color.x = 1.0f;
    }

    // create bullets
    for(int i = 0; i < 20000; i++) {
        u32 entity = CreateEntity(&ecs);
        Transform* t = AddComponent(&ecs, entity, TRANSFORM_ID);
        t->position.x = rand() % app.windowWidth;
        t->position.y = rand() % app.windowHeight*100;
        t->scale.x = 128 + rand() % 128;
        t->scale.y = t->scale.x;
        t->scale.z = 1.0f;

        Rigidbody* r = AddComponent(&ecs, entity, RIGIDBODY_ID);
        r->velocity.y = -100.0f;

        Mesh* mesh = AddComponent(&ecs, entity, MESH_ID);
        mesh->model = &model;

        Material* m = AddComponent(&ecs, entity, MATERIAL_ID);
        m->color.x = ( rand() % 100 ) * 0.01f;
        m->color.y = ( rand() % 100 ) * 0.01f;
        m->color.z = ( rand() % 100 ) * 0.01f;
        m->color.w = 1.0f;

        m->image = &iconImage;
        m->shader = shaderProgram;
    }

    f32 gameTime = 0.0f;
    f32 deltaTime = 0.0f;

    ECSView view = InitECSView(TRANSFORM_ID | RIGIDBODY_ID | MATERIAL_ID, 128);
    
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
        //ForEachEntityWithComponents(&ecs, TRANSFORM_ID | RIGIDBODY_ID | MATERIAL_ID, UpdateBullets, &count);
        UpdateECSView(&ecs, &view);
        for (u32 i = 0; i < vec_count(&view.entities); i++) {
            u32 entity = view.entities[i];
            Transform* t = GetComponent(&ecs, entity, TRANSFORM_ID);
            Rigidbody* rb = GetComponent(&ecs, entity, RIGIDBODY_ID);

            t->position.y += rb->velocity.y * deltaTime;
        }
        

        Matrix4 projection = Mat4Orthographic(0.0f, (float)app.windowWidth, 0.0f, (float)app.windowHeight, 0.001f, 100.0f); 
        Matrix4 cameraView = IdentityMatrix4(); 
        Mat4Translate(&cameraView, InitVector3(/*sin((double)SDL_GetTicks()) * 2*/0.0f, 0.0f, -0.5f));

        for (u32 i = 0; i < vec_count(&view.entities); i++) {
            u32 entity = view.entities[i];
            Transform* t = GetComponent(&ecs, entity, TRANSFORM_ID);
            Mesh* mesh = GetComponent(&ecs, entity, MESH_ID);
            Material* material = GetComponent(&ecs, entity, MATERIAL_ID);
            
            Matrix4 transform = IdentityMatrix4();
            Mat4Translate(&transform, t->position);
            Mat4Scale(&transform, t->scale);

            
            BindShader(material->shader);
            ShaderSetFloat(material->shader, "TIME", SDL_GetTicks()/1000.0f);
            ShaderSetMatrix4(material->shader, "VIEW", cameraView);
            ShaderSetMatrix4(material->shader, "PROJECTION", projection);

            ShaderBindTexture(material->shader, iconImage.id, "MAIN_TEXTURE", 0);
            ShaderSetVector4(material->shader, "COLOR", material->color);
            ShaderSetMatrix4(material->shader, "TRANSFORM", transform);
            DrawModel(*mesh->model);
        }

        glFlush();

        SwapWindow(&app);

        printf("FPS: %f %i\n", 1.0f/deltaTime, count);
    }

    FreeECSView(&view);

    FreeModel(model);

    free(iconImage.data);
    free(containerImage.data);

    FreeWindow(&app);

    DeleteShader(shaderProgram);
    return 0;
}
