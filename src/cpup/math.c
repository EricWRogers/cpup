#include "math.h"

Vector3 InitVector3(f32 _x, f32 _y, f32 _z)
{
    Vector3 v;
    v.x = _x;
    v.y = _y;
    v.z = _z;
    return v;
}

bool CampareMatrix4(Matrix4* _a, Matrix4* _b)
{
    return memcmp(_a, _b, sizeof(Matrix4)) == 0;
}

Matrix4 IdentityMatrix4()
{
    Matrix4 matrix = {};

    matrix.m[0] = 1.0f;
    matrix.m[4] = 0.0f;
    matrix.m[8] = 0.0f;
    matrix.m[12] = 0.0f;
    matrix.m[1] = 0.0f;
    matrix.m[5] = 1.0f;
    matrix.m[9] = 0.0f;
    matrix.m[13] = 0.0f;
    matrix.m[2] = 0.0f;
    matrix.m[6] = 0.0f;
    matrix.m[10] = 1.0f;
    matrix.m[14] = 0.0f;
    matrix.m[3] = 0.0f;
    matrix.m[7] = 0.0f;
    matrix.m[11] = 0.0f;
    matrix.m[15] = 1.0f;

    return matrix;
}

void Mat4Add(Matrix4* _mat, f32 _scalar)
{
    for (int i = 0; i < 16; ++i)
        (*_mat).m[i] = (*_mat).m[i] + _scalar;
}

void Mat4Sub(Matrix4* _mat, f32 _scalar)
{
    for (int i = 0; i < 16; ++i)
        (*_mat).m[i] = (*_mat).m[i] - _scalar;
}

void Mat4Mul(Matrix4* _mat, f32 _scalar)
{
    for (int i = 0; i < 16; ++i)
        (*_mat).m[i] = (*_mat).m[i] * _scalar;
}

void Mat4Div(Matrix4* _mat, f32 _scalar)
{
    for (int i = 0; i < 16; ++i)
        (*_mat).m[i] = (*_mat).m[i] / _scalar;
}

void Mat4Translate(Matrix4* _mat, Vector3 _translation)
{
    (*_mat).m[12] = _translation.x; // (0,3)
    (*_mat).m[13] = _translation.y; // (1,3)
    (*_mat).m[14] = _translation.z; // (2,3)
}

void Mat4Scale(Matrix4* _mat, Vector3 _scale)
{
    (*_mat).m[0] *= _scale.x;  // (0,0)
    (*_mat).m[5] *= _scale.y;  // (1,1)
    (*_mat).m[10] *= _scale.z; // (2,2)
    (*_mat).m[15] = 1.0f;     // (3,3)
}

Matrix4 Mat4Orthographic(f32 _left, f32 _right, f32 _bottom, f32 _top, f32 _near, f32 _far)
{
    Matrix4 matrix;

    f32 rl = (_right - _left);
    f32 tb = (_top - _bottom);
    f32 fn = (_far - _near);

    for (int i = 0; i < 16; ++i)
        matrix.m[i] = 0.0f;

    matrix.m[0] = 2.0f / rl;   // (0,0)
    matrix.m[5] = 2.0f / tb;   // (1,1)
    matrix.m[10] = -2.0f / fn; // (2,2)
    matrix.m[15] = 1.0f;       // (3,3)

    // Translation (last column)
    matrix.m[12] = -(_right + _left) / rl; // (0,3)
    matrix.m[13] = -(_top + _bottom) / tb; // (1,3)
    matrix.m[14] = -(_far + _near) / fn;   // (2,3)

    return matrix;
}