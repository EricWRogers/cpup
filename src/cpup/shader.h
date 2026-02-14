#pragma once
#include "types.h"

u32 GenerateShaderFromFiles(const char* _vertexPath, const char* _fragmentPath);
u32 GenerateShader(const char* _vertexShaderSource, const char* _fragmentShaderSource);

void DeleteShader(u32 _shaderID);

void BindShader(u32 _shaderID);
void UnBindShader();
void ShaderBindTexture(u32 _shaderID, u32 _textureID, const char* _variableName, u32 _slot);
void ShaderSetFloat(u32 _shaderID, const char* _variableName, f32 _value);