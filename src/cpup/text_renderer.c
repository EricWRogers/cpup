#include "text_renderer.h"

#include "opengl.h"

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

typedef stbtt_bakedchar TextGlyph;

static const i32 kFirstGlyph = 32;
static const i32 kGlyphCount = 96;

void InitTextRenderer2D(
    TextRenderer2D* _renderer,
    SpriteRenderer2D* _spriteRenderer,
    const char* _fontPath,
    f32 _fontPixelHeight,
    u32 _atlasWidth,
    u32 _atlasHeight
) {
    if (_renderer == NULL)
        return;

    memset(_renderer, 0, sizeof(TextRenderer2D));
    _renderer->spriteRenderer = _spriteRenderer;
    _renderer->atlasWidth = _atlasWidth ? _atlasWidth : 512;
    _renderer->atlasHeight = _atlasHeight ? _atlasHeight : 512;
    _renderer->fontPixelHeight = _fontPixelHeight > 0.0f ? _fontPixelHeight : 32.0f;

    _renderer->glyphData = calloc((size_t)kGlyphCount, sizeof(TextGlyph));
    if (_renderer->glyphData == NULL) {
        printf("Text renderer: failed to allocate glyph table.\n");
        return;
    }

    size_t fontSize = 0;
    void* fontData = SDL_LoadFile(_fontPath, &fontSize);
    if (fontData == NULL || fontSize == 0) {
        printf("Text renderer: failed to load font %s.\n", _fontPath);
        return;
    }

    const size_t atlasPixels = (size_t)_renderer->atlasWidth * (size_t)_renderer->atlasHeight;
    unsigned char* atlasMono = calloc(atlasPixels, sizeof(unsigned char));
    if (atlasMono == NULL) {
        printf("Text renderer: failed to allocate mono atlas.\n");
        SDL_free(fontData);
        return;
    }

    i32 bakedRows = stbtt_BakeFontBitmap(
        (const unsigned char*)fontData,
        0,
        _renderer->fontPixelHeight,
        atlasMono,
        (i32)_renderer->atlasWidth,
        (i32)_renderer->atlasHeight,
        kFirstGlyph,
        kGlyphCount,
        (TextGlyph*)_renderer->glyphData
    );
    SDL_free(fontData);

    if (bakedRows <= 0) {
        printf("Text renderer: failed to bake font atlas.\n");
        free(atlasMono);
        return;
    }

    unsigned char* atlasRgba = malloc(atlasPixels * 4);
    if (atlasRgba == NULL) {
        printf("Text renderer: failed to allocate RGBA atlas.\n");
        free(atlasMono);
        return;
    }

    for (size_t i = 0; i < atlasPixels; i++) {
        atlasRgba[i * 4 + 0] = 255;
        atlasRgba[i * 4 + 1] = 255;
        atlasRgba[i * 4 + 2] = 255;
        atlasRgba[i * 4 + 3] = atlasMono[i];
    }
    free(atlasMono);

    glGenTextures(1, &_renderer->textureId);
    glBindTexture(GL_TEXTURE_2D, _renderer->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        (i32)_renderer->atlasWidth,
        (i32)_renderer->atlasHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        atlasRgba
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    free(atlasRgba);
}

void FreeTextRenderer2D(TextRenderer2D* _renderer) {
    if (_renderer == NULL)
        return;

    if (_renderer->textureId != 0)
        glDeleteTextures(1, &_renderer->textureId);
    if (_renderer->glyphData != NULL)
        free(_renderer->glyphData);

    memset(_renderer, 0, sizeof(TextRenderer2D));
}

void RenderText(
    TextRenderer2D* _renderer,
    const char* _text,
    f32 _x,
    f32 _y,
    f32 _depth,
    f32 _scale,
    Vector4 _color,
    f32 _screenHeight
) {
    if (_renderer == NULL || _renderer->spriteRenderer == NULL || _renderer->glyphData == NULL || _text == NULL)
        return;
    if (_renderer->textureId == 0 || _scale <= 0.0f || _screenHeight <= 0.0f)
        return;

    f32 penX = _x;
    f32 penY = _y;
    f32 lineStartX = _x;
    f32 lineStep = _renderer->fontPixelHeight * _scale;
    TextGlyph* glyphs = (TextGlyph*)_renderer->glyphData;

    for (const char* c = _text; *c != '\0'; c++) {
        if (*c == '\n') {
            penX = lineStartX;
            penY += lineStep;
            continue;
        }

        i32 codepoint = (unsigned char)*c;
        if (codepoint < kFirstGlyph || codepoint >= (kFirstGlyph + kGlyphCount))
            continue;

        TextGlyph glyph = glyphs[codepoint - kFirstGlyph];

        // Text layout is top-left origin with +Y down.
        f32 left = penX + (glyph.xoff * _scale);
        f32 top = penY + (glyph.yoff * _scale);
        f32 right = left + ((glyph.x1 - glyph.x0) * _scale);
        f32 bottom = top + ((glyph.y1 - glyph.y0) * _scale);

        // Convert to world coordinates where +Y goes up.
        f32 worldTop = _screenHeight - top;
        f32 worldBottom = _screenHeight - bottom;

        Vector3 position = {
            .x = (left + right) * 0.5f,
            .y = (worldTop + worldBottom) * 0.5f,
            .z = _depth
        };
        Vector2 size = {
            .x = right - left,
            .y = worldTop - worldBottom
        };

        f32 tTop = (f32)glyph.y0 / (f32)_renderer->atlasHeight;
        f32 tBottom = (f32)glyph.y1 / (f32)_renderer->atlasHeight;
        Vector2 uvMin = {
            .x = (f32)glyph.x0 / (f32)_renderer->atlasWidth,
            .y = tBottom
        };
        Vector2 uvMax = {
            .x = (f32)glyph.x1 / (f32)_renderer->atlasWidth,
            .y = tTop
        };

        SpriteRendererDrawUV(
            _renderer->spriteRenderer,
            position,
            size,
            uvMin,
            uvMax,
            _renderer->textureId,
            _depth,
            _color
        );

        penX += glyph.xadvance * _scale;
    }
}
