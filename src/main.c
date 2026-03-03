#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/ecs.h"
#include "cpup/vec.h"
#include "cpup/math.h"
#include "cpup/types.h"
#include "cpup/model.h"
#include "cpup/opengl.h"
#include "cpup/shader.h"
#include "cpup/input_manager.h"
#include "cpup/sprite_renderer.h"
#include "cpup/text_renderer.h"
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

typedef struct {
    f32 timer;
} TimeBomb;

u64 TRANSFORM_ID;
u64 RIGIDBODY_ID;
u64 MESH_ID;
u64 MATERIAL_ID;
u64 TIME_BOMB_ID;

void UpdateBullets(ECS *_ecs, u32 _entityId, void *_userData) {
    Transform* transform = GetComponent(_ecs, _entityId, TRANSFORM_ID);
    Rigidbody* rigidbody = GetComponent(_ecs, _entityId, RIGIDBODY_ID);
    int* count = _userData;
    *count += 1;
}

u32 SpawnLogo(ECS* _ecs) {
    u32 entity = CreateEntity(_ecs);
    Transform* t = AddComponent(_ecs, entity, TRANSFORM_ID);
    Rigidbody* r = AddComponent(_ecs, entity, RIGIDBODY_ID);
    Mesh* mesh = AddComponent(_ecs, entity, MESH_ID);
    Material* m = AddComponent(_ecs, entity, MATERIAL_ID);
    TimeBomb* bomb = AddComponent(_ecs, entity, TIME_BOMB_ID);
    return entity;
}

void ResetLogo(ECS* _ecs, u32 _entity, u32 _shaderId, Image* _image, Model* _model) {
    Transform* t = GetComponent(_ecs, _entity, TRANSFORM_ID);
    t->position.x = 300.0f;
    t->position.y = 300.0f;
    t->scale.x = 128 + rand() % 128;
    t->scale.y = t->scale.x;
    t->scale.z = 1.0f;

    Rigidbody* r = GetComponent(_ecs, _entity, RIGIDBODY_ID);
    r->velocity.x = ((rand() % 200) - 100) * 0.01f;
    r->velocity.y = ((rand() % 200) - 100) * 0.01f;
    r->velocity = Vec3Mul(r->velocity, 100.0f);

    Mesh* mesh = GetComponent(_ecs, _entity, MESH_ID);
    mesh->model = _model;

    Material* m = GetComponent(_ecs, _entity, MATERIAL_ID);
    m->color.x = ( rand() % 100 ) * 0.01f;
    m->color.y = ( rand() % 100 ) * 0.01f;
    m->color.z = ( rand() % 100 ) * 0.01f;
    m->color.w = 1.0f;

    m->image = _image;
    m->shader = _shaderId;

    TimeBomb* bomb = GetComponent(_ecs, _entity, TIME_BOMB_ID);
    bomb->timer = 5.0f;
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
    TIME_BOMB_ID = RegisterComponent(&ecs, sizeof(TimeBomb));

    u32 entity1 = CreateEntity(&ecs);

    ECSView view = InitECSView(TRANSFORM_ID | RIGIDBODY_ID | MATERIAL_ID, 128);
    SpriteRenderer2D spriteRenderer;
    TextRenderer2D textRenderer;
    InitSpriteRenderer2D(&spriteRenderer, 1024, "assets/shaders/sprite.vs", "assets/shaders/sprite.fs");
    InitTextRenderer2D(
        &textRenderer,
        &spriteRenderer,
        "assets/fonts/Antonio-Bold.ttf",
        32.0f,
        512,
        512
    );
    
    bool running = true;
    f32 time = 0.0f;
    while(running) {
        InputManagerNewFrame(&app);

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

        for(int i = 0; i < 5 && GetAliveEntityCount(&ecs) < 50000; i++) {
            u32 entity  = SpawnLogo(&ecs);
            ResetLogo(&ecs, entity, shaderProgram, &iconImage, &model);
        }

        if (app.time != 0.0f)
            app.deltaTime = (SDL_GetTicksNS() * 1e-9) -  app.time;
        
        app.time = SDL_GetTicksNS() * 1e-9;
        //UpdateECSView(&ecs, &view);
        for (u32 i = 0; i < vec_count(&view.entities); i++) {
            u32 entity = view.entities[i];
            Transform* t = GetComponent(&ecs, entity, TRANSFORM_ID);
            Rigidbody* rb = GetComponent(&ecs, entity, RIGIDBODY_ID);
            t->position = Vec3Add(t->position, Vec3Mul(rb->velocity, app.deltaTime));
            TimeBomb* bomb = GetComponent(&ecs, entity, TIME_BOMB_ID);
            bomb->timer -= app.deltaTime;
            
            if (bomb->timer <= 0.0f)
                ResetLogo(&ecs, entity, shaderProgram, &iconImage, &model);
        }
        

        Matrix4 projection = Mat4Orthographic(0.0f, (float)app.windowWidth, 0.0f, (float)app.windowHeight, 0.001f, 100.0f); 
        Matrix4 cameraView = IdentityMatrix4(); 
        Mat4Translate(&cameraView, InitVector3(/*sin((double)SDL_GetTicks()) * 2*/0.0f, 0.0f, -0.5f));

        Transform* t;
        Material* material;

        SpriteRendererBegin(&spriteRenderer, SPRITE_SORT_TEXTURE);
        UpdateECSView(&ecs, &view);
        for (u32 i = 0; i < vec_count(&view.entities); i++) {
            u32 entity = view.entities[i];
            t = GetComponent(&ecs, entity, TRANSFORM_ID);
            material = GetComponent(&ecs, entity, MATERIAL_ID);

            u32 textureId = material->image ? material->image->id : iconImage.id;
            SpriteRendererDraw(
                &spriteRenderer,
                t->position,
                (Vector2){ .x = t->scale.x, .y = t->scale.y },
                textureId,
                -1,
                material->color
            );
        }
        char debugText[128];
        snprintf(
            debugText,
            sizeof(debugText),
            "FPS: %.1f  Alive: %u",
            1.0f / app.deltaTime,
            GetAliveEntityCount(&ecs)
        );
        RenderText(
            &textRenderer,
            debugText,
            10.0f,
            25.0f,
            -0.9f,
            0.75f,
            (Vector4){ .x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 1.0f },
            (f32)app.windowHeight
        );

        SpriteRendererEnd(&spriteRenderer);
        SpriteRendererRender(&spriteRenderer, projection, SDL_GetTicks()/1000.0f);

        SwapWindow(&app);
    }

    FreeECSView(&view);
    FreeSpriteRenderer2D(&spriteRenderer);
    FreeTextRenderer2D(&textRenderer);

    FreeModel(model);

    free(iconImage.data);
    free(containerImage.data);

    FreeWindow(&app);

    DeleteShader(shaderProgram);
    return 0;
}
