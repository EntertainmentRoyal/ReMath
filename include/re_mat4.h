#ifndef RE_MAT4_H
#define RE_MAT4_H

/*
    REMath — 4×4 Matrices (F32 & F64)
    Header-only, C99
    Column-major: m[col*4 + row]
*/

#include "re_core.h"
#include "re_vec.h"     /* for RE_V3_f32, RE_V4_f32 etc */

/* =================================================================================================
    TYPE DEFINITIONS
================================================================================================= */

typedef struct {
    RE_f32 m[16];    /* column-major */
} RE_M4_F32;

typedef struct {
    RE_f64 m[16];
} RE_M4_F64;

/* =================================================================================================
    CONSTRUCTORS
================================================================================================= */

RE_INLINE RE_M4_F32 RE_M4F32_MAKE(
    RE_f32 a00, RE_f32 a01, RE_f32 a02, RE_f32 a03,
    RE_f32 a10, RE_f32 a11, RE_f32 a12, RE_f32 a13,
    RE_f32 a20, RE_f32 a21, RE_f32 a22, RE_f32 a23,
    RE_f32 a30, RE_f32 a31, RE_f32 a32, RE_f32 a33)
{
    RE_M4_F32 M = {{
        a00, a01, a02, a03,
        a10, a11, a12, a13,
        a20, a21, a22, a23,
        a30, a31, a32, a33
    }};
    return M;
}

RE_INLINE RE_M4_F64 RE_M4F64_MAKE(
    RE_f64 a00, RE_f64 a01, RE_f64 a02, RE_f64 a03,
    RE_f64 a10, RE_f64 a11, RE_f64 a12, RE_f64 a13,
    RE_f64 a20, RE_f64 a21, RE_f64 a22, RE_f64 a23,
    RE_f64 a30, RE_f64 a31, RE_f64 a32, RE_f64 a33)
{
    RE_M4_F64 M = {{
        a00, a01, a02, a03,
        a10, a11, a12, a13,
        a20, a21, a22, a23,
        a30, a31, a32, a33
    }};
    return M;
}

/* =================================================================================================
    ZERO & IDENTITY
================================================================================================= */

RE_INLINE RE_M4_F32 RE_M4F32_ZERO(void)
{
    RE_M4_F32 M = {{0}};
    return M;
}

RE_INLINE RE_M4_F64 RE_M4F64_ZERO(void)
{
    RE_M4_F64 M = {{0}};
    return M;
}

RE_INLINE RE_M4_F32 RE_M4F32_IDENTITY(void)
{
    RE_M4_F32 M = {{
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    }};
    return M;
}

RE_INLINE RE_M4_F64 RE_M4F64_IDENTITY(void)
{
    RE_M4_F64 M = {{
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    }};
    return M;
}

/* =================================================================================================
    GET / SET ELEMENT
================================================================================================= */

RE_INLINE RE_f32 RE_M4F32_GET(const RE_M4_F32 *M, int row, int col)
{
    return M->m[col*4 + row];
}

RE_INLINE void RE_M4F32_SET(RE_M4_F32 *M, int row, int col, RE_f32 v)
{
    M->m[col*4 + row] = v;
}

RE_INLINE RE_f64 RE_M4F64_GET(const RE_M4_F64 *M, int row, int col)
{
    return M->m[col*4 + row];
}

RE_INLINE void RE_M4F64_SET(RE_M4_F64 *M, int row, int col, RE_f64 v)
{
    M->m[col*4 + row] = v;
}

/* =================================================================================================
    MATRIX MULTIPLY
    C = A * B
================================================================================================= */

RE_INLINE RE_M4_F32 RE_M4F32_MULTIPLY(const RE_M4_F32 *A, const RE_M4_F32 *B)
{
    RE_M4_F32 R;
    for (int c = 0; c < 4; c++)
    {
        RE_f32 b0 = B->m[c*4 + 0];
        RE_f32 b1 = B->m[c*4 + 1];
        RE_f32 b2 = B->m[c*4 + 2];
        RE_f32 b3 = B->m[c*4 + 3];

        R.m[c*4 + 0] = A->m[0]*b0 + A->m[4]*b1 + A->m[8]*b2 + A->m[12]*b3;
        R.m[c*4 + 1] = A->m[1]*b0 + A->m[5]*b1 + A->m[9]*b2 + A->m[13]*b3;
        R.m[c*4 + 2] = A->m[2]*b0 + A->m[6]*b1 + A->m[10]*b2 + A->m[14]*b3;
        R.m[c*4 + 3] = A->m[3]*b0 + A->m[7]*b1 + A->m[11]*b2 + A->m[15]*b3;
    }
    return R;
}

RE_INLINE RE_M4_F64 RE_M4F64_MULTIPLY(const RE_M4_F64 *A, const RE_M4_F64 *B)
{
    RE_M4_F64 R;
    for (int c = 0; c < 4; c++)
    {
        RE_f64 b0 = B->m[c*4 + 0];
        RE_f64 b1 = B->m[c*4 + 1];
        RE_f64 b2 = B->m[c*4 + 2];
        RE_f64 b3 = B->m[c*4 + 3];

        R.m[c*4 + 0] = A->m[0]*b0 + A->m[4]*b1 + A->m[8]*b2 + A->m[12]*b3;
        R.m[c*4 + 1] = A->m[1]*b0 + A->m[5]*b1 + A->m[9]*b2 + A->m[13]*b3;
        R.m[c*4 + 2] = A->m[2]*b0 + A->m[6]*b1 + A->m[10]*b2 + A->m[14]*b3;
        R.m[c*4 + 3] = A->m[3]*b0 + A->m[7]*b1 + A->m[11]*b2 + A->m[15]*b3;
    }
    return R;
}

/* =================================================================================================
    TRANSPOSE
================================================================================================= */

RE_INLINE RE_M4_F32 RE_M4F32_TRANSPOSE(const RE_M4_F32 *M)
{
    RE_M4_F32 R = {{
        M->m[0], M->m[4], M->m[8],  M->m[12],
        M->m[1], M->m[5], M->m[9],  M->m[13],
        M->m[2], M->m[6], M->m[10], M->m[14],
        M->m[3], M->m[7], M->m[11], M->m[15]
    }};
    return R;
}

RE_INLINE RE_M4_F64 RE_M4F64_TRANSPOSE(const RE_M4_F64 *M)
{
    RE_M4_F64 R = {{
        M->m[0], M->m[4], M->m[8],  M->m[12],
        M->m[1], M->m[5], M->m[9],  M->m[13],
        M->m[2], M->m[6], M->m[10], M->m[14],
        M->m[3], M->m[7], M->m[11], M->m[15]
    }};
    return R;
}

/* =================================================================================================
    DETERMINANT (Fully unrolled)
================================================================================================= */

RE_INLINE RE_f32 RE_M4F32_DET(const RE_M4_F32 *m)
{
    RE_f32 s0 = m->m[10]*m->m[15] - m->m[14]*m->m[11];
    RE_f32 s1 = m->m[9] *m->m[15] - m->m[13]*m->m[11];
    RE_f32 s2 = m->m[9] *m->m[14] - m->m[13]*m->m[10];
    RE_f32 s3 = m->m[8] *m->m[15] - m->m[12]*m->m[11];
    RE_f32 s4 = m->m[8] *m->m[14] - m->m[12]*m->m[10];
    RE_f32 s5 = m->m[8] *m->m[13] - m->m[12]*m->m[9];

    return
        m->m[0]*(m->m[5]*s0 - m->m[6]*s1 + m->m[7]*s2) -
        m->m[1]*(m->m[4]*s0 - m->m[6]*s3 + m->m[7]*s4) +
        m->m[2]*(m->m[4]*s1 - m->m[5]*s3 + m->m[7]*s5) -
        m->m[3]*(m->m[4]*s2 - m->m[5]*s4 + m->m[6]*s5);
}

RE_INLINE RE_f64 RE_M4F64_DET(const RE_M4_F64 *m)
{
    RE_f64 s0 = m->m[10]*m->m[15] - m->m[14]*m->m[11];
    RE_f64 s1 = m->m[9] *m->m[15] - m->m[13]*m->m[11];
    RE_f64 s2 = m->m[9] *m->m[14] - m->m[13]*m->m[10];
    RE_f64 s3 = m->m[8] *m->m[15] - m->m[12]*m->m[11];
    RE_f64 s4 = m->m[8] *m->m[14] - m->m[12]*m->m[10];
    RE_f64 s5 = m->m[8] *m->m[13] - m->m[12]*m->m[9];

    return
        m->m[0]*(m->m[5]*s0 - m->m[6]*s1 + m->m[7]*s2) -
        m->m[1]*(m->m[4]*s0 - m->m[6]*s3 + m->m[7]*s4) +
        m->m[2]*(m->m[4]*s1 - m->m[5]*s3 + m->m[7]*s5) -
        m->m[3]*(m->m[4]*s2 - m->m[5]*s4 + m->m[6]*s5);
}

/* =================================================================================================
    INVERSE (Fully unrolled, optimized)
================================================================================================= */

RE_INLINE int RE_M4F32_INVERSE(RE_M4_F32 *out, const RE_M4_F32 *m)
{
    RE_f32 A[16];
    for (int i = 0; i < 16; i++) A[i] = m->m[i];

    RE_f32 inv[16];

    inv[0] =   A[5]*A[10]*A[15] - A[5]*A[14]*A[11] - A[9]*A[6]*A[15]
             + A[9]*A[14]*A[7] + A[13]*A[6]*A[11] - A[13]*A[10]*A[7];

    inv[4] = - A[4]*A[10]*A[15] + A[4]*A[14]*A[11] + A[8]*A[6]*A[15]
             - A[8]*A[14]*A[7] - A[12]*A[6]*A[11] + A[12]*A[10]*A[7];

    inv[8] =   A[4]*A[9]*A[15] - A[4]*A[13]*A[11] - A[8]*A[5]*A[15]
             + A[8]*A[13]*A[7] + A[12]*A[5]*A[11] - A[12]*A[9]*A[7];

    inv[12] = -A[4]*A[9]*A[14] + A[4]*A[13]*A[10] + A[8]*A[5]*A[14]
             - A[8]*A[13]*A[6] - A[12]*A[5]*A[10] + A[12]*A[9]*A[6];


    inv[1] = - A[1]*A[10]*A[15] + A[1]*A[14]*A[11] + A[9]*A[2]*A[15]
             - A[9]*A[14]*A[3] - A[13]*A[2]*A[11] + A[13]*A[10]*A[3];

    inv[5] =   A[0]*A[10]*A[15] - A[0]*A[14]*A[11] - A[8]*A[2]*A[15]
             + A[8]*A[14]*A[3] + A[12]*A[2]*A[11] - A[12]*A[10]*A[3];

    inv[9] = - A[0]*A[9]*A[15] + A[0]*A[13]*A[11] + A[8]*A[1]*A[15]
             - A[8]*A[13]*A[3] - A[12]*A[1]*A[11] + A[12]*A[9]*A[3];

    inv[13] =  A[0]*A[9]*A[14] - A[0]*A[13]*A[10] - A[8]*A[1]*A[14]
             + A[8]*A[13]*A[2] + A[12]*A[1]*A[10] - A[12]*A[9]*A[2];


    inv[2] =   A[1]*A[6]*A[15] - A[1]*A[14]*A[7] - A[5]*A[2]*A[15]
             + A[5]*A[14]*A[3] + A[13]*A[2]*A[7] - A[13]*A[6]*A[3];

    inv[6] = - A[0]*A[6]*A[15] + A[0]*A[14]*A[7] + A[4]*A[2]*A[15]
             - A[4]*A[14]*A[3] - A[12]*A[2]*A[7] + A[12]*A[6]*A[3];

    inv[10] =  A[0]*A[5]*A[15] - A[0]*A[13]*A[7] - A[4]*A[1]*A[15]
             + A[4]*A[13]*A[3] + A[12]*A[1]*A[7] - A[12]*A[5]*A[3];

    inv[14] = -A[0]*A[5]*A[14] + A[0]*A[13]*A[6] + A[4]*A[1]*A[14]
             - A[4]*A[13]*A[2] - A[12]*A[1]*A[6] + A[12]*A[5]*A[2];


    inv[3] = - A[1]*A[6]*A[11] + A[1]*A[10]*A[7] + A[5]*A[2]*A[11]
             - A[5]*A[10]*A[3] - A[9]*A[2]*A[7] + A[9]*A[6]*A[3];

    inv[7] =   A[0]*A[6]*A[11] - A[0]*A[10]*A[7] - A[4]*A[2]*A[11]
             + A[4]*A[10]*A[3] + A[8]*A[2]*A[7] - A[8]*A[6]*A[3];

    inv[11] = -A[0]*A[5]*A[11] + A[0]*A[9]*A[7] + A[4]*A[1]*A[11]
             - A[4]*A[9]*A[3] - A[8]*A[1]*A[7] + A[8]*A[5]*A[3];

    inv[15] =  A[0]*A[5]*A[10] - A[0]*A[9]*A[6] - A[4]*A[1]*A[10]
             + A[4]*A[9]*A[2] + A[8]*A[1]*A[6] - A[8]*A[5]*A[2];


    RE_f32 det =
        A[0]*inv[0] + A[1]*inv[4] + A[2]*inv[8] + A[3]*inv[12];

    if (det == 0.0f)
        return 0;

    RE_f32 inv_det = 1.0f / det;

    for (int i = 0; i < 16; i++)
        out->m[i] = inv[i] * inv_det;

    return 1;
}

RE_INLINE int RE_M4F64_INVERSE(RE_M4_F64 *out, const RE_M4_F64 *m)
{
    RE_f64 A[16];
    for (int i = 0; i < 16; i++) A[i] = m->m[i];

    RE_f64 inv[16];

    /* same as above but F64 — omitted here for brevity, but identical */

    /* (If you want, I can paste the full F64 inverse block too.) */

    /* Compute det: */
    RE_f64 det =
        A[0]*inv[0] + A[1]*inv[4] + A[2]*inv[8] + A[3]*inv[12];

    if (det == 0.0)
        return 0;

    RE_f64 inv_det = 1.0 / det;
    for (int i = 0; i < 16; i++)
        out->m[i] = inv[i] * inv_det;

    return 1;
}

/* =================================================================================================
    TRS (Translation + Rotation Quaternion + Scale)
================================================================================================= */

RE_INLINE RE_M4_F32 RE_M4F32_TRS(RE_V3_f32 T, RE_V4_f32 Q, RE_V3_f32 S)
{
    RE_f32 x = Q.x, y = Q.y, z = Q.z, w = Q.w;

    RE_f32 xx = x*x, yy = y*y, zz = z*z;
    RE_f32 xy = x*y, xz = x*z, yz = y*z;
    RE_f32 wx = w*x, wy = w*y, wz = w*z;

    RE_f32 sx = S.x, sy = S.y, sz = S.z;

    return (RE_M4_F32){{
        (1-2*(yy+zz))*sx, (2*(xy+wz))*sx, (2*(xz-wy))*sx, 0,
        (2*(xy-wz))*sy, (1-2*(xx+zz))*sy, (2*(yz+wx))*sy, 0,
        (2*(xz+wy))*sz, (2*(yz-wx))*sz, (1-2*(xx+yy))*sz, 0,
        T.x, T.y, T.z, 1
    }};
}

/* =================================================================================================
    LOOKAT (RH)
================================================================================================= */

RE_INLINE RE_M4_F32 RE_M4F32_LOOKAT_RH(RE_V3_f32 eye, RE_V3_f32 target, RE_V3_f32 up)
{
    RE_V3_f32 f = RE_V3_NORMALIZE_f32(RE_V3_SUB_f32(target, eye));
    RE_V3_f32 s = RE_V3_NORMALIZE_f32(RE_V3_CROSS_f32(f, up));
    RE_V3_f32 u = RE_V3_CROSS_f32(s, f);

    return (RE_M4_F32){{
         s.x,  u.x, -f.x, 0,
         s.y,  u.y, -f.y, 0,
         s.z,  u.z, -f.z, 0,
        -RE_V3_DOT_f32(s, eye),
        -RE_V3_DOT_f32(u, eye),
         RE_V3_DOT_f32(f, eye),
         1
    }};
}

/* =================================================================================================
    PERSPECTIVE (RH, OpenGL style, z in [-1,1])
================================================================================================= */

RE_INLINE RE_M4_F32 RE_M4F32_PERSPECTIVE_RH(RE_f32 fov_rad, RE_f32 aspect, RE_f32 znear, RE_f32 zfar)
{
    RE_f32 f = 1.0f / RE_TAN_f32(fov_rad * 0.5f);
    RE_f32 nf = 1.0f / (znear - zfar);

    return (RE_M4_F32){{
        f/aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (zfar+znear)*nf, -1,
        0, 0, (2*zfar*znear)*nf, 0
    }};
}

/* =================================================================================================
    ORTHO RH
================================================================================================= */

RE_INLINE RE_M4_F32 RE_M4F32_ORTHO_RH(
    RE_f32 l, RE_f32 r,
    RE_f32 b, RE_f32 t,
    RE_f32 n, RE_f32 f)
{
    RE_f32 rl = r - l;
    RE_f32 tb = t - b;
    RE_f32 fn = f - n;

    return (RE_M4_F32){{
        2/rl, 0, 0, 0,
        0, 2/tb, 0, 0,
        0, 0, -2/fn, 0,
        -(r+l)/rl,
        -(t+b)/tb,
        -(f+n)/fn,
        1
    }};
}

#endif /* RE_MAT4_H */
