#pragma once
#include <string.h>
#include "types.h"

typedef struct {
    i32 id;
    i32 width;
    i32 height;
    u32* data;
} Image;

Image LoadImage(const char* _path);

char* LoadFile(const char* _path);

int     get_int();
float   get_float();
char    get_char();
char*   get_string();

int     request_int(const char* _message);
float   request_float(const char* _message);
char    request_char(const char* _message);
char*   request_string(const char* _message);

int     random_int(int _min, int _max);
float   random_float(float _min, float _max);
