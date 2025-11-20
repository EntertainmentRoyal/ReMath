#ifndef RE_MAT3_H
#define RE_MAT3_H

/*
    REMath — 3×3 Matrices (F32 & F64)
    Header-only, C99
*/

#include "re_core.h"

/* ================================================================================================
    DATA TYPES
================================================================================================ */

typedef struct {
    RE_f32 m[9];   /* column-major: m[col*3 + row] */
} RE_M3_F32;

typedef struct {
    RE_f64 m[9];
} RE_M3_F64;

/* ================================================================================================
    CONSTRUCTORS
================================================================================================ */

RE_INLINE RE_M3_F32 RE_M3F32_MAKE(
    RE_f32 a00, RE_f32 a01, RE_f32 a02,
    RE_f32 a10, RE_f32 a11, RE_f32 a12,
    RE_f32 a20, RE_f32 a21, RE_f32 a22)
{
    RE_M3_F32 M = { {
        a00, a01, a02,
        a10, a11, a12,
        a20, a21, a22
    }};
    return M;
}

RE_INLINE RE_M3_F64 RE_M3F64_MAKE(
    RE_f64 a00, RE_f64 a01, RE_f64 a02,
    RE_f64 a10, RE_f64 a11, RE_f64 a12,
    RE_f64 a20, RE_f64 a21, RE_f64 a22)
{
    RE_M3_F64 M = { {
        a00, a01, a02,
        a10, a11, a12,
        a20, a21, a22
    }};
    return M;
}

/* ================================================================================================
    ZERO / IDENTITY
================================================================================================ */

RE_INLINE RE_M3_F32 RE_M3F32_ZERO(void) {
    RE_M3_F32 M = { {0,0,0, 0,0,0, 0,0,0} };
    return M;
}
RE_INLINE RE_M3_F64 RE_M3F64_ZERO(void) {
    RE_M3_F64 M = { {0,0,0, 0,0,0, 0,0,0} };
    return M;
}

RE_INLINE RE_M3_F32 RE_M3F32_IDENTITY(void) {
    RE_M3_F32 M = { {
        1,0,0,
        0,1,0,
        0,0,1
    }};
    return M;
}
RE_INLINE RE_M3_F64 RE_M3F64_IDENTITY(void) {
    RE_M3_F64 M = { {
        1,0,0,
        0,1,0,
        0,0,1
    }};
    return M;
}

/* ================================================================================================
    GET / SET ELEMENT
================================================================================================ */

RE_INLINE RE_f32 RE_M3F32_GET(const RE_M3_F32 *M, int row, int col)
{
    return M->m[col*3 + row];
}

RE_INLINE void RE_M3F32_SET(RE_M3_F32 *M, int row, int col, RE_f32 v)
{
    M->m[col*3 + row] = v;
}

RE_INLINE RE_f64 RE_M3F64_GET(const RE_M3_F64 *M, int row, int col)
{
    return M->m[col*3 + row];
}

RE_INLINE void RE_M3F64_SET(RE_M3_F64 *M, int row, int col, RE_f64 v)
{
    M->m[col*3 + row] = v;
}

/* ================================================================================================
    DET
================================================================================================ */

RE_INLINE RE_f32 RE_M3F32_DET(const RE_M3_F32 *M)
{
    return
        M->m[0]*(M->m[4]*M->m[8] - M->m[5]*M->m[7]) -
        M->m[3]*(M->m[1]*M->m[8] - M->m[2]*M->m[7]) +
        M->m[6]*(M->m[1]*M->m[5] - M->m[2]*M->m[4]);
}

RE_INLINE RE_f64 RE_M3F64_DET(const RE_M3_F64 *M)
{
    return
        M->m[0]*(M->m[4]*M->m[8] - M->m[5]*M->m[7]) -
        M->m[3]*(M->m[1]*M->m[8] - M->m[2]*M->m[7]) +
        M->m[6]*(M->m[1]*M->m[5] - M->m[2]*M->m[4]);
}

/* ================================================================================================
    INVERSE
================================================================================================ */

RE_INLINE int RE_M3F32_INVERSE(RE_M3_F32 *out, const RE_M3_F32 *M)
{
    RE_f32 c00 =  M->m[4]*M->m[8] - M->m[5]*M->m[7];
    RE_f32 c01 = -(M->m[3]*M->m[8] - M->m[5]*M->m[6]);
    RE_f32 c02 =  M->m[3]*M->m[7] - M->m[4]*M->m[6];

    RE_f32 c10 = -(M->m[1]*M->m[8] - M->m[2]*M->m[7]);
    RE_f32 c11 =  M->m[0]*M->m[8] - M->m[2]*M->m[6];
    RE_f32 c12 = -(M->m[0]*M->m[7] - M->m[1]*M->m[6]);

    RE_f32 c20 =  M->m[1]*M->m[5] - M->m[2]*M->m[4];
    RE_f32 c21 = -(M->m[0]*M->m[5] - M->m[2]*M->m[3]);
    RE_f32 c22 =  M->m[0]*M->m[4] - M->m[1]*M->m[3];

    RE_f32 det =
        M->m[0]*c00 + M->m[3]*c10 + M->m[6]*c20;

    if (det == 0.0f) return 0;

    RE_f32 inv = 1.0f / det;

    out->m[0] = c00 * inv;
    out->m[1] = c10 * inv;
    out->m[2] = c20 * inv;

    out->m[3] = c01 * inv;
    out->m[4] = c11 * inv;
    out->m[5] = c21 * inv;

    out->m[6] = c02 * inv;
    out->m[7] = c12 * inv;
    out->m[8] = c22 * inv;

    return 1;
}

RE_INLINE int RE_M3F64_INVERSE(RE_M3_F64 *out, const RE_M3_F64 *M)
{
    RE_f64 c00 =  M->m[4]*M->m[8] - M->m[5]*M->m[7];
    RE_f64 c01 = -(M->m[3]*M->m[8] - M->m[5]*M->m[6]);
    RE_f64 c02 =  M->m[3]*M->m[7] - M->m[4]*M->m[6];

    RE_f64 c10 = -(M->m[1]*M->m[8] - M->m[2]*M->m[7]);
    RE_f64 c11 =  M->m[0]*M->m[8] - M->m[2]*M->m[6];
    RE_f64 c12 = -(M->m[0]*M->m[7] - M->m[1]*M->m[6]);

    RE_f64 c20 =  M->m[1]*M->m[5] - M->m[2]*M->m[4];
    RE_f64 c21 = -(M->m[0]*M->m[5] - M->m[2]*M->m[3]);
    RE_f64 c22 =  M->m[0]*M->m[4] - M->m[1]*M->m[3];

    RE_f64 det =
        M->m[0]*c00 + M->m[3]*c10 + M->m[6]*c20;

    if (det == 0.0) return 0;

    RE_f64 inv = 1.0 / det;

    out->m[0] = c00 * inv;
    out->m[1] = c10 * inv;
    out->m[2] = c20 * inv;

    out->m[3] = c01 * inv;
    out->m[4] = c11 * inv;
    out->m[5] = c21 * inv;

    out->m[6] = c02 * inv;
    out->m[7] = c12 * inv;
    out->m[8] = c22 * inv;

    return 1;
}

/* ================================================================================================
    MULTIPLY  (C = A * B)
================================================================================================ */

RE_INLINE RE_M3_F32 RE_M3F32_MULTIPLY(const RE_M3_F32 *A, const RE_M3_F32 *B)
{
    RE_M3_F32 C;

    for (int col = 0; col < 3; col++) {
        C.m[col*3 + 0] = A->m[0]*B->m[col*3+0] + A->m[3]*B->m[col*3+1] + A->m[6]*B->m[col*3+2];
        C.m[col*3 + 1] = A->m[1]*B->m[col*3+0] + A->m[4]*B->m[col*3+1] + A->m[7]*B->m[col*3+2];
        C.m[col*3 + 2] = A->m[2]*B->m[col*3+0] + A->m[5]*B->m[col*3+1] + A->m[8]*B->m[col*3+2];
    }

    return C;
}

RE_INLINE RE_M3_F64 RE_M3F64_MULTIPLY(const RE_M3_F64 *A, const RE_M3_F64 *B)
{
    RE_M3_F64 C;

    for (int col = 0; col < 3; col++) {
        C.m[col*3 + 0] = A->m[0]*B->m[col*3+0] + A->m[3]*B->m[col*3+1] + A->m[6]*B->m[col*3+2];
        C.m[col*3 + 1] = A->m[1]*B->m[col*3+0] + A->m[4]*B->m[col*3+1] + A->m[7]*B->m[col*3+2];
        C.m[col*3 + 2] = A->m[2]*B->m[col*3+0] + A->m[5]*B->m[col*3+1] + A->m[8]*B->m[col*3+2];
    }

    return C;
}

/* ================================================================================================
    TRANSPOSE
================================================================================================ */

RE_INLINE RE_M3_F32 RE_M3F32_TRANSPOSE(const RE_M3_F32 *M)
{
    RE_M3_F32 R = {
        {
            M->m[0], M->m[3], M->m[6],
            M->m[1], M->m[4], M->m[7],
            M->m[2], M->m[5], M->m[8]
        }
    };
    return R;
}

RE_INLINE RE_M3_F64 RE_M3F64_TRANSPOSE(const RE_M3_F64 *M)
{
    RE_M3_F64 R = {
        {
            M->m[0], M->m[3], M->m[6],
            M->m[1], M->m[4], M->m[7],
            M->m[2], M->m[5], M->m[8]
        }
    };
    return R;
}

#endif /* RE_MAT3_H */
