#include "sprite_renderer.h"

#include "opengl.h"
#include "shader.h"
#include "vec.h"

#include <string.h>

static void SpriteRendererCreateVertexArray(SpriteRenderer2D* _renderer) {
    if (_renderer->vao == 0)
        glGenVertexArrays(1, &_renderer->vao);
    if (_renderer->vbo == 0)
        glGenBuffers(1, &_renderer->vbo);
    if (_renderer->ebo == 0)
        glGenBuffers(1, &_renderer->ebo);

    glBindVertexArray(_renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, _renderer->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _renderer->ebo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)(3 * sizeof(f32)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)(7 * sizeof(f32)));

    glBindVertexArray(0);
}

void InitSpriteRenderer2D(SpriteRenderer2D* _renderer, u32 _capacity, const char* _vertexShaderPath, const char* _fragmentShaderPath) {
    u32 capacity = _capacity ? _capacity : 1;

    _renderer->glyphs = vec_init(capacity, sizeof(SpriteGlyph));
    _renderer->vertices = vec_init(capacity * 4, sizeof(SpriteVertex));
    _renderer->indices = vec_init(capacity * 6, sizeof(u32));
    _renderer->renderBatches = vec_init(capacity, sizeof(SpriteRenderBatch));

    _renderer->vao = 0;
    _renderer->vbo = 0;
    _renderer->ebo = 0;
    _renderer->shader = GenerateShaderFromFiles(_vertexShaderPath, _fragmentShaderPath);
    _renderer->sortType = SPRITE_SORT_TEXTURE;
    _renderer->time = 0.0f;

    SpriteRendererCreateVertexArray(_renderer);
}

void FreeSpriteRenderer2D(SpriteRenderer2D* _renderer) {
    if (_renderer->vao != 0)
        glDeleteVertexArrays(1, &_renderer->vao);
    if (_renderer->vbo != 0)
        glDeleteBuffers(1, &_renderer->vbo);
    if (_renderer->ebo != 0)
        glDeleteBuffers(1, &_renderer->ebo);
    if (_renderer->shader != 0)
        DeleteShader(_renderer->shader);

    if (_renderer->glyphs != NULL)
        vec_free(&_renderer->glyphs);
    if (_renderer->vertices != NULL)
        vec_free(&_renderer->vertices);
    if (_renderer->indices != NULL)
        vec_free(&_renderer->indices);
    if (_renderer->renderBatches != NULL)
        vec_free(&_renderer->renderBatches);
}

void SpriteRendererBegin(SpriteRenderer2D* _renderer, SpriteSortType _sortType) {
    _renderer->sortType = _sortType;
    vec_clear(&_renderer->glyphs);
    vec_clear(&_renderer->renderBatches);
}

void SpriteRendererDraw(
    SpriteRenderer2D* _renderer,
    Vector3 _position,
    Vector2 _size,
    u32 _textureId,
    f32 _depth,
    Vector4 _color
) {
    Vector2 uvMin = { .x = 0.0f, .y = 0.0f };
    Vector2 uvMax = { .x = 1.0f, .y = 1.0f };
    SpriteRendererDrawUV(_renderer, _position, _size, uvMin, uvMax, _textureId, _depth, _color);
}

void SpriteRendererDrawUV(
    SpriteRenderer2D* _renderer,
    Vector3 _position,
    Vector2 _size,
    Vector2 _uvMin,
    Vector2 _uvMax,
    u32 _textureId,
    f32 _depth,
    Vector4 _color
) {
    Vector2 halfSize = { .x = _size.x * 0.5f, .y = _size.y * 0.5f };
    f32 left = _position.x - halfSize.x;
    f32 right = _position.x + halfSize.x;
    f32 bottom = _position.y - halfSize.y;
    f32 top = _position.y + halfSize.y;

    SpriteGlyph glyph;
    glyph.textureId = _textureId;
    glyph.depth = _depth;

    glyph.topLeft.position = (Vector3){ .x = left, .y = top, .z = _depth };
    glyph.topLeft.color = _color;
    glyph.topLeft.uv = (Vector2){ .x = _uvMin.x, .y = _uvMax.y };

    glyph.bottomLeft.position = (Vector3){ .x = left, .y = bottom, .z = _depth };
    glyph.bottomLeft.color = _color;
    glyph.bottomLeft.uv = (Vector2){ .x = _uvMin.x, .y = _uvMin.y };

    glyph.bottomRight.position = (Vector3){ .x = right, .y = bottom, .z = _depth };
    glyph.bottomRight.color = _color;
    glyph.bottomRight.uv = (Vector2){ .x = _uvMax.x, .y = _uvMin.y };

    glyph.topRight.position = (Vector3){ .x = right, .y = top, .z = _depth };
    glyph.topRight.color = _color;
    glyph.topRight.uv = (Vector2){ .x = _uvMax.x, .y = _uvMax.y };

    vec_add(&_renderer->glyphs, &glyph);
}

static bool SpriteGlyphShouldSwap(const SpriteGlyph* _a, const SpriteGlyph* _b, SpriteSortType _sortType) {
    switch (_sortType) {
        case SPRITE_SORT_FRONT_TO_BACK:
            return _a->depth > _b->depth;
        case SPRITE_SORT_BACK_TO_FRONT:
            return _a->depth < _b->depth;
        case SPRITE_SORT_TEXTURE:
        default:
            return _a->textureId > _b->textureId;
    }
}

static void SpriteRendererSortGlyphs(SpriteRenderer2D* _renderer) {
    u32 count = vec_count(&_renderer->glyphs);
    for (u32 i = 1; i < count; i++) {
        SpriteGlyph key = _renderer->glyphs[i];
        i32 j = (i32)i - 1;
        while (j >= 0 && SpriteGlyphShouldSwap(&_renderer->glyphs[j], &key, _renderer->sortType)) {
            _renderer->glyphs[j + 1] = _renderer->glyphs[j];
            j--;
        }
        _renderer->glyphs[j + 1] = key;
    }
}

static void SpriteRendererCreateRenderBatches(SpriteRenderer2D* _renderer) {
    u32 glyphCount = vec_count(&_renderer->glyphs);
    if (glyphCount == 0)
        return;

    vec_resize(&_renderer->vertices, glyphCount * 4);
    vec_resize(&_renderer->indices, glyphCount * 6);
    vec_clear(&_renderer->renderBatches);

    u32 vertexIndex = 0;
    u32 indexIndex = 0;
    for (u32 i = 0; i < glyphCount; i++) {
        SpriteGlyph* glyph = &_renderer->glyphs[i];

        _renderer->vertices[vertexIndex + 0] = glyph->topRight;
        _renderer->vertices[vertexIndex + 1] = glyph->bottomRight;
        _renderer->vertices[vertexIndex + 2] = glyph->bottomLeft;
        _renderer->vertices[vertexIndex + 3] = glyph->topLeft;

        _renderer->indices[indexIndex + 0] = vertexIndex + 0;
        _renderer->indices[indexIndex + 1] = vertexIndex + 1;
        _renderer->indices[indexIndex + 2] = vertexIndex + 2;
        _renderer->indices[indexIndex + 3] = vertexIndex + 0;
        _renderer->indices[indexIndex + 4] = vertexIndex + 2;
        _renderer->indices[indexIndex + 5] = vertexIndex + 3;

        if (i == 0 || glyph->textureId != _renderer->glyphs[i - 1].textureId) {
            SpriteRenderBatch batch = {
                .indexOffset = indexIndex,
                .indexCount = 6,
                .textureId = glyph->textureId
            };
            vec_add(&_renderer->renderBatches, &batch);
        } else {
            _renderer->renderBatches[vec_count(&_renderer->renderBatches) - 1].indexCount += 6;
        }

        vertexIndex += 4;
        indexIndex += 6;
    }

    glBindVertexArray(_renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, _renderer->vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vec_count(&_renderer->vertices) * sizeof(SpriteVertex),
        _renderer->vertices,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _renderer->ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        vec_count(&_renderer->indices) * sizeof(u32),
        _renderer->indices,
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

void SpriteRendererEnd(SpriteRenderer2D* _renderer) {
    if (vec_count(&_renderer->glyphs) == 0)
        return;

    SpriteRendererSortGlyphs(_renderer);
    SpriteRendererCreateRenderBatches(_renderer);
}

void SpriteRendererRender(SpriteRenderer2D* _renderer, Matrix4 _projection, f32 _time) {
    if (vec_count(&_renderer->renderBatches) == 0 || _renderer->shader == 0)
        return;

    _renderer->time = _time;

    BindShader(_renderer->shader);
    ShaderSetFloat(_renderer->shader, "TIME", _renderer->time);
    ShaderSetMatrix4(_renderer->shader, "P", _projection);

    glBindVertexArray(_renderer->vao);
    for (u32 i = 0; i < vec_count(&_renderer->renderBatches); i++) {
        SpriteRenderBatch batch = _renderer->renderBatches[i];
        ShaderBindTexture(_renderer->shader, batch.textureId, "mySampler", 0);
        glDrawElements(
            GL_TRIANGLES,
            batch.indexCount,
            GL_UNSIGNED_INT,
            (void*)(batch.indexOffset * sizeof(u32))
        );
    }
    glBindVertexArray(0);
    UnBindShader();
}
