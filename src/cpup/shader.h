#pragma once
#include "types.h"

u32 GenerateShader(const char* _vertexShaderSource, const char* _fragmentShaderSource);

void ShaderUse(u32 _shaderID);
void ShaderBindTexture(u32 _shaderID, u32 _textureID, const char* _variableName, u32 _slot);