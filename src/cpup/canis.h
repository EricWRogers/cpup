#pragma once

#include "types.h"

typedef struct {
    void* window;
    void* glContext;
} AppContext;

extern i32 InitCanis();