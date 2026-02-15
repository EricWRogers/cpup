#pragma once

#include "types.h"

typedef struct {
    void* window;
    void* glContext;
    i32 windowWidth;
    i32 windowHeight;
} AppContext;

extern i32 InitCanis();