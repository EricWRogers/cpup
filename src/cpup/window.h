#pragma once
#include "canis.h"

i32 InitWindow(AppContext* _appContext);

void FreeWindow(AppContext* _appContext);

void SwapWindow(AppContext* _appContext);

void SetWindowTitle(AppContext* _appContext, const char* _name);

void ClearWindow();