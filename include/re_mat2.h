#ifndef RE_MAT2_H
#define RE_MAT2_H

/*
    REMath — 2×2 Matrices (F32 & F64)
    Header-only, C99, safe and compact.
*/

#include "re_core.h"

/* ================================================================================================
    DATA TYPES
================================================================================================ */

typedef struct {
    RE_f32 m[4];   /* column-major: m[col*2 + row] */
} RE_M2_F32;

typedef struct {
    RE_f64 m[4];
} RE_M2_F64;

/* ================================================================================================
    CONSTRUCTORS
================================================================================================ */

RE_INLINE RE_M2_F32 RE_M2F32_MAKE(RE_f32 a00, RE_f32 a01,
                                      RE_f32 a10, RE_f32 a11)
{
    RE_M2_F32 M = { { a00, a01, a10, a11 } };
    return M;
}

RE_INLINE RE_M2_F64 RE_M2F64_MAKE(RE_f64 a00, RE_f64 a01,
                                      RE_f64 a10, RE_f64 a11)
{
    RE_M2_F64 M = { { a00, a01, a10, a11 } };
    return M;
}

/* ================================================================================================
    ZERO / IDENTITY
================================================================================================ */

RE_INLINE RE_M2_F32 RE_M2F32_ZERO(void) {
    RE_M2_F32 M = { { 0,0,0,0 } };
    return M;
}
RE_INLINE RE_M2_F64 RE_M2F64_ZERO(void) {
    RE_M2_F64 M = { { 0,0,0,0 } };
    return M;
}

RE_INLINE RE_M2_F32 RE_M2F32_IDENTITY(void) {
    RE_M2_F32 M = { { 1,0,0,1 } };
    return M;
}
RE_INLINE RE_M2_F64 RE_M2F64_IDENTITY(void) {
    RE_M2_F64 M = { { 1,0,0,1 } };
    return M;
}

/* ================================================================================================
    GET / SET ELEMENT
================================================================================================ */

RE_INLINE RE_f32 RE_M2F32_GET(const RE_M2_F32 *M, int row, int col)
{
    return M->m[col*2 + row];
}

RE_INLINE void RE_M2F32_SET(RE_M2_F32 *M, int row, int col, RE_f32 v)
{
    M->m[col*2 + row] = v;
}

RE_INLINE RE_f64 RE_M2F64_GET(const RE_M2_F64 *M, int row, int col)
{
    return M->m[col*2 + row];
}

RE_INLINE void RE_M2F64_SET(RE_M2_F64 *M, int row, int col, RE_f64 v)
{
    M->m[col*2 + row] = v;
}

/* ================================================================================================
    DETERMINANT
================================================================================================ */

RE_INLINE RE_f32 RE_M2F32_DET(const RE_M2_F32 *M)
{
    return M->m[0] * M->m[3] - M->m[2] * M->m[1];
}

RE_INLINE RE_f64 RE_M2F64_DET(const RE_M2_F64 *M)
{
    return M->m[0] * M->m[3] - M->m[2] * M->m[1];
}

/* ================================================================================================
    INVERSE
================================================================================================ */

RE_INLINE int RE_M2F32_INVERSE(RE_M2_F32 *out, const RE_M2_F32 *M)
{
    RE_f32 det = RE_M2F32_DET(M);
    if (det == 0.0f) return 0;

    RE_f32 inv = 1.0f / det;

    out->m[0] =  M->m[3] * inv;
    out->m[1] = -M->m[1] * inv;
    out->m[2] = -M->m[2] * inv;
    out->m[3] =  M->m[0] * inv;

    return 1;
}

RE_INLINE int RE_M2F64_INVERSE(RE_M2_F64 *out, const RE_M2_F64 *M)
{
    RE_f64 det = RE_M2F64_DET(M);
    if (det == 0.0) return 0;

    RE_f64 inv = 1.0 / det;

    out->m[0] =  M->m[3] * inv;
    out->m[1] = -M->m[1] * inv;
    out->m[2] = -M->m[2] * inv;
    out->m[3] =  M->m[0] * inv;

    return 1;
}

/* ================================================================================================
    MULTIPLY  (C = A * B)
================================================================================================ */

RE_INLINE RE_M2_F32 RE_M2F32_MULTIPLY(const RE_M2_F32 *A, const RE_M2_F32 *B)
{
    RE_M2_F32 C;

    C.m[0] = A->m[0]*B->m[0] + A->m[2]*B->m[1];
    C.m[1] = A->m[1]*B->m[0] + A->m[3]*B->m[1];

    C.m[2] = A->m[0]*B->m[2] + A->m[2]*B->m[3];
    C.m[3] = A->m[1]*B->m[2] + A->m[3]*B->m[3];

    return C;
}

RE_INLINE RE_M2_F64 RE_M2F64_MULTIPLY(const RE_M2_F64 *A, const RE_M2_F64 *B)
{
    RE_M2_F64 C;

    C.m[0] = A->m[0]*B->m[0] + A->m[2]*B->m[1];
    C.m[1] = A->m[1]*B->m[0] + A->m[3]*B->m[1];

    C.m[2] = A->m[0]*B->m[2] + A->m[2]*B->m[3];
    C.m[3] = A->m[1]*B->m[2] + A->m[3]*B->m[3];

    return C;
}

/* ================================================================================================
    TRANSPOSE
================================================================================================ */

RE_INLINE RE_M2_F32 RE_M2F32_TRANSPOSE(const RE_M2_F32 *M)
{
    RE_M2_F32 R = {
        M->m[0], M->m[2],
        M->m[1], M->m[3]
    };
    return R;
}

RE_INLINE RE_M2_F64 RE_M2F64_TRANSPOSE(const RE_M2_F64 *M)
{
    RE_M2_F64 R = {
        M->m[0], M->m[2],
        M->m[1], M->m[3]
    };
    return R;
}

#endif /* RE_MAT2_H */
