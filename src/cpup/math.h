#pragma once
#include "types.h"

typedef struct {
    f32 x;
    f32 y;
} Vector2;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vector3;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct {
    f32 m[9];
} Matrix3;

typedef struct {
    f32 m[16];
} Matrix4;

Vector3 InitVector3(f32 _x, f32 _y, f32 _z);

bool CampareMatrix4(Matrix4* _a, Matrix4* _b);

Matrix4 IdentityMatrix4();

void Mat4Add(Matrix4* _mat, f32 _scalar);

void Mat4Sub(Matrix4* _mat, f32 _scalar);

void Mat4Mul(Matrix4* _mat, f32 _scalar);

void Mat4Div(Matrix4* _mat, f32 _scalar);

void Mat4Translate(Matrix4* _mat, Vector3 _translation);

void Mat4Scale(Matrix4* _mat, Vector3 _scale);

Matrix4 Mat4Orthographic(f32 _left, f32 _right, f32 _bottom, f32 _top, f32 _near, f32 _far);