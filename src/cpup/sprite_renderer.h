#pragma once

#include "math.h"
#include "types.h"

typedef enum {
    SPRITE_SORT_TEXTURE,
    SPRITE_SORT_FRONT_TO_BACK,
    SPRITE_SORT_BACK_TO_FRONT,
} SpriteSortType;

typedef struct {
    Vector3 position;
    Vector4 color;
    Vector2 uv;
} SpriteVertex;

typedef struct {
    SpriteVertex topRight;
    SpriteVertex bottomRight;
    SpriteVertex bottomLeft;
    SpriteVertex topLeft;
    u32 textureId;
    f32 depth;
} SpriteGlyph;

typedef struct {
    u32 indexOffset;
    u32 indexCount;
    u32 textureId;
} SpriteRenderBatch;

typedef struct {
    SpriteGlyph* glyphs;
    SpriteVertex* vertices;
    u32* indices;
    SpriteRenderBatch* renderBatches;
    u32 vao;
    u32 vbo;
    u32 ebo;
    u32 shader;
    SpriteSortType sortType;
    f32 time;
} SpriteRenderer2D;

void InitSpriteRenderer2D(SpriteRenderer2D* _renderer, u32 _capacity, const char* _vertexShaderPath, const char* _fragmentShaderPath);
void FreeSpriteRenderer2D(SpriteRenderer2D* _renderer);

void SpriteRendererBegin(SpriteRenderer2D* _renderer, SpriteSortType _sortType);
void SpriteRendererDraw(
    SpriteRenderer2D* _renderer,
    Vector3 _position, // Sprite center in world space.
    Vector2 _size,
    u32 _textureId,
    f32 _depth,
    Vector4 _color
);
void SpriteRendererEnd(SpriteRenderer2D* _renderer);
void SpriteRendererRender(SpriteRenderer2D* _renderer, Matrix4 _projection, f32 _time);
