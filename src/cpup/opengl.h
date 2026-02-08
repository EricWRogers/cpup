#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>
const static char* OPENGLVERSION = "#version 300 es";
#else
#include <GL/glew.h>
const static char* OPENGLVERSION = "#version 330 core"; 
#endif