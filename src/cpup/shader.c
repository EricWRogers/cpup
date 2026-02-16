#include "shader.h"

#include "io.h"
#include "opengl.h"

#include <stdio.h>
#include <stdlib.h>

u32 GenerateShaderFromFiles(const char* _vertexPath, const char* _fragmentPath)
{
    char* vertexSource = LoadFile(_vertexPath);

    if (vertexSource == NULL)
        return 0;
    
    char* fragmentSource = LoadFile(_fragmentPath);

    if (fragmentSource == NULL)
    {
        free(vertexSource);
        return 0;
    }

    u32 shaderId = GenerateShader(vertexSource, fragmentSource);

    free(vertexSource);
    free(fragmentSource);

    return shaderId;
}

u32 GenerateShader(const char* _vertexShaderSource, const char* _fragmentShaderSource)
{
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &_vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
        return 0;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &_fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glDeleteShader(vertexShader);
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
        return 0;
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

    return shaderProgram;
}

void DeleteShader(u32 _shaderID)
{
    glDeleteShader(_shaderID);
}

void BindShader(u32 _shaderID)
{
    glUseProgram(_shaderID);
}

void UnBindShader()
{
    BindShader(0);
}

void ShaderBindTexture(u32 _shaderID, u32 _textureID, const char* _variableName, u32 _slot)
{
    // load the texture into a slot
    glBindTexture(GL_TEXTURE_2D + _slot, _textureID); 
    // get texture variable from sharder then tell it which slot to use
    glUniform1i(glGetUniformLocation(_shaderID, _variableName), _slot);
}

void ShaderSetFloat(u32 _shaderID, const char* _variableName, f32 _value)
{
    i32 location = glGetUniformLocation(_shaderID, _variableName);
    if (location > -1)
        glUniform1f( location, _value);
}

void ShaderSetVector3(u32 _shaderID, const char* _variableName, Vector3 _vec)
{
    i32 location = glGetUniformLocation(_shaderID, _variableName);
    if (location > -1)
        glUniform2fv( location, 1, &_vec.x); 
}

void ShaderSetMatrix4(u32 _shaderID, const char* _variableName, Matrix4 _mat)
{
    i32 location = glGetUniformLocation(_shaderID, _variableName);
    if (location > -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, _mat.m);
}