#include "model.h"

#include "vec.h"
#include "opengl.h"

Model BuildModel(f32** _vertices, u32** _indices, DrawInfo _info)
{
    Model model;
    model.vertices = *_vertices;
    model.indices = *_indices;

    _vertices = NULL;
    _indices = NULL;

    glGenVertexArrays(1, &model.VAO);
    glGenBuffers(1, &model.VBO);
    glGenBuffers(1, &model.EBO);

    glBindVertexArray(model.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
    glBufferData(GL_ARRAY_BUFFER, vec_size_of((void*)&model.vertices), model.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vec_size_of((void*)&model.indices), model.indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return model;
}

void DrawModel(Model _model)
{
    glBindVertexArray(_model.VAO);
    glDrawElements(GL_TRIANGLES, vec_count(&_model.indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void FreeModel(Model _model)
{
    glDeleteVertexArrays(1, &_model.VAO);
    glDeleteBuffers(1, &_model.VBO);
    glDeleteBuffers(1, &_model.EBO);
    vec_free(&_model.vertices);
    vec_free(&_model.indices);
}