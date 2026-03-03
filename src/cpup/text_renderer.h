#pragma once

#include "sprite_renderer.h"

typedef struct {
    u32 textureId;
    u32 atlasWidth;
    u32 atlasHeight;
    f32 fontPixelHeight;
    void* glyphData;
    SpriteRenderer2D* spriteRenderer;
} TextRenderer2D;

void InitTextRenderer2D(
    TextRenderer2D* _renderer,
    SpriteRenderer2D* _spriteRenderer,
    const char* _fontPath,
    f32 _fontPixelHeight,
    u32 _atlasWidth,
    u32 _atlasHeight
);

void FreeTextRenderer2D(TextRenderer2D* _renderer);

void RenderText(
    TextRenderer2D* _renderer,
    const char* _text,
    f32 _x,
    f32 _y,
    f32 _depth,
    f32 _scale,
    Vector4 _color,
    f32 _screenHeight
);
