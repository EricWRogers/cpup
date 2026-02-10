#pragma once
#include "canis.h"

i32 window_init(AppContext* _appContext);

void window_destroy(AppContext* _appContext);

void window_swap(AppContext* _appContext);

void window_clear();