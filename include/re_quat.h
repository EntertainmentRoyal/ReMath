#ifndef RE_QUAT_H
#define RE_QUAT_H

#include "re_core.h"
#include "re_math.h"
#include "re_math_ext.h"
#include "re_vec.h"

/* ================================================================
   QUAT TYPES
   ================================================================ */

typedef struct { RE_f32 x, y, z, w; } RE_QUAT_f32;
typedef struct { RE_f64 x, y, z, w; } RE_QUAT_f64;


/* ================================================================
   QUAT CONSTRUCTORS
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_MAKE_f32(RE_f32 x, RE_f32 y, RE_f32 z, RE_f32 w)
{
    RE_QUAT_f32 q = { x,y,z,w };
    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_MAKE_f64(RE_f64 x, RE_f64 y, RE_f64 z, RE_f64 w)
{
    RE_QUAT_f64 q = { x,y,z,w };
    return q;
}

RE_INLINE RE_QUAT_f32 RE_QUAT_IDENTITY_f32(void)
{
    RE_QUAT_f32 q = {0,0,0,1};
    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_IDENTITY_f64(void)
{
    RE_QUAT_f64 q = {0,0,0,1};
    return q;
}


/* ================================================================
   BASIC OPS
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_ADD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    RE_QUAT_f32 q = {
        a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w
    };
    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_ADD_f64(RE_QUAT_f64 a, RE_QUAT_f64 b)
{
    RE_QUAT_f64 q = {
        a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w
    };
    return q;
}

RE_INLINE RE_QUAT_f32 RE_QUAT_MUL_SCALAR_f32(RE_QUAT_f32 q, RE_f32 s)
{
    RE_QUAT_f32 r = { q.x*s, q.y*s, q.z*s, q.w*s };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_MUL_SCALAR_f64(RE_QUAT_f64 q, RE_f64 s)
{
    RE_QUAT_f64 r = { q.x*s, q.y*s, q.z*s, q.w*s };
    return r;
}


/* ================================================================
   HAMILTON PRODUCT (q1 * q2)
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_MUL_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    RE_QUAT_f32 q;

    q.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
    q.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
    q.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
    q.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;

    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_MUL_f64(RE_QUAT_f64 a, RE_QUAT_f64 b)
{
    RE_QUAT_f64 q;

    q.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
    q.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
    q.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
    q.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;

    return q;
}


/* ================================================================
   LENGTH / NORMALIZE
   ================================================================ */

RE_INLINE RE_f32 RE_QUAT_LENGTH_f32(RE_QUAT_f32 q)
{
    return RE_SQRT_FAST_f32(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

RE_INLINE RE_f64 RE_QUAT_LENGTH_f64(RE_QUAT_f64 q)
{
    return RE_SQRT(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

RE_INLINE RE_QUAT_f32 RE_QUAT_NORMALIZE_f32(RE_QUAT_f32 q)
{
    RE_f32 l = RE_QUAT_LENGTH_f32(q);
    if (l <= 0.0f) return RE_QUAT_IDENTITY_f32();
    RE_f32 inv = RE_RCP(l);

    RE_QUAT_f32 r = { q.x*inv, q.y*inv, q.z*inv, q.w*inv };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_NORMALIZE_f64(RE_QUAT_f64 q)
{
    RE_f64 l = RE_QUAT_LENGTH_f64(q);
    if (l <= 0.0f) return RE_QUAT_IDENTITY_f64();
    RE_f64 inv = 1.0 / l;

    RE_QUAT_f64 r = { q.x*inv, q.y*inv, q.z*inv, q.w*inv };
    return r;
}


/* ================================================================
   FROM AXIS-ANGLE
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_FROM_AXIS_ANGLE_f32(RE_V3_f32 axis, RE_f32 angle)
{
    RE_f32 s = RE_SIN_f32(0.5f * angle);
    RE_f32 c = RE_COS_f32(0.5f * angle);

    RE_V3_f32 na = RE_V3_NORMALIZE_f32(axis);

    RE_QUAT_f32 q = { na.x*s, na.y*s, na.z*s, c };
    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_FROM_AXIS_ANGLE_f64(RE_V3_f64 axis, RE_f64 angle)
{
    RE_f64 s = RE_SIN_f32(angle * 0.5);
    RE_f64 c = RE_COS_f32(angle * 0.5);

    RE_f64 len = RE_SQRT(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
    if (len == 0.0) return RE_QUAT_IDENTITY_f64();

    RE_f64 inv = 1.0 / len;

    RE_QUAT_f64 q = {
        axis.x*inv*s,
        axis.y*inv*s,
        axis.z*inv*s,
        c
    };
    return q;
}


/* ================================================================
   QUAT → MAT4
   (Used by your TRS and LookAt)
   ================================================================ */

RE_INLINE void RE_QUAT_TO_MAT4_f32(RE_QUAT_f32 q, RE_f32 out[16])
{
    RE_f32 x=q.x, y=q.y, z=q.z, w=q.w;

    RE_f32 xx=x*x, yy=y*y, zz=z*z;
    RE_f32 xy=x*y, xz=x*z, yz=y*z;
    RE_f32 wx=w*x, wy=w*y, wz=w*z;

    out[ 0] = 1 - 2*(yy + zz);
    out[ 1] = 2*(xy + wz);
    out[ 2] = 2*(xz - wy);
    out[ 3] = 0;

    out[ 4] = 2*(xy - wz);
    out[ 5] = 1 - 2*(xx + zz);
    out[ 6] = 2*(yz + wx);
    out[ 7] = 0;

    out[ 8] = 2*(xz + wy);
    out[ 9] = 2*(yz - wx);
    out[10] = 1 - 2*(xx + yy);
    out[11] = 0;

    out[12]=0; out[13]=0; out[14]=0; out[15]=1;
}

RE_INLINE void RE_QUAT_TO_MAT4_f64(RE_QUAT_f64 q, RE_f64 out[16])
{
    RE_f64 x=q.x, y=q.y, z=q.z, w=q.w;

    RE_f64 xx=x*x, yy=y*y, zz=z*z;
    RE_f64 xy=x*y, xz=x*z, yz=y*z;
    RE_f64 wx=w*x, wy=w*y, wz=w*z;

    out[ 0] = 1 - 2*(yy + zz);
    out[ 1] = 2*(xy + wz);
    out[ 2] = 2*(xz - wy);
    out[ 3] = 0;

    out[ 4] = 2*(xy - wz);
    out[ 5] = 1 - 2*(xx + zz);
    out[ 6] = 2*(yz + wx);
    out[ 7] = 0;

    out[ 8] = 2*(xz + wy);
    out[ 9] = 2*(yz - wx);
    out[10] = 1 - 2*(xx + yy);
    out[11] = 0;

    out[12]=0; out[13]=0; out[14]=0; out[15]=1;
}


/* ================================================================
   SLERP (Stable, branch-minimized)
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_SLERP_f32(RE_QUAT_f32 a, RE_QUAT_f32 b, RE_f32 t)
{
    RE_f32 dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;

    /* ensure shortest path */
    if (dot < 0.0f) {
        dot = -dot;
        b.x=-b.x; b.y=-b.y; b.z=-b.z; b.w=-b.w;
    }

    const RE_f32 DOT_T = 0.9995f;

    if (dot > DOT_T)
    {
        /* linear fallback */
        RE_QUAT_f32 q = {
            a.x + t*(b.x-a.x),
            a.y + t*(b.y-a.y),
            a.z + t*(b.z-a.z),
            a.w + t*(b.w-a.w)
        };
        return RE_QUAT_NORMALIZE_f32(q);
    }

    RE_f32 th = RE_ACOS(dot);
    RE_f32 s = RE_RCP(RE_SIN_f32(th));

    RE_f32 t0 = RE_SIN_f32((1.0f-t)*th) * s;
    RE_f32 t1 = RE_SIN_f32(t*th) * s;

    RE_QUAT_f32 q = {
        a.x*t0 + b.x*t1,
        a.y*t0 + b.y*t1,
        a.z*t0 + b.z*t1,
        a.w*t0 + b.w*t1
    };
    return q;
}

/* ================================================================
   SIMD DETECTION
   ================================================================ */

#if defined(__AVX__)
    #define RE_SIMD_AVX 1
#elif defined(__SSE2__) || defined(_MSC_VER)
    #define RE_SIMD_SSE 1
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    #define RE_SIMD_NEON 1
#else
    #define RE_SIMD_NONE 1
#endif


/* ================================================================
   SIMD: QUAT DOT
   ================================================================ */

#if RE_SIMD_AVX
#include <immintrin.h>

RE_INLINE RE_f32 RE_QUAT_DOT_SIMD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    __m128 qa = _mm_loadu_ps(&a.x);
    __m128 qb = _mm_loadu_ps(&b.x);
    __m128 mul = _mm_mul_ps(qa, qb);

    __m128 sum1 = _mm_hadd_ps(mul, mul);
    __m128 sum2 = _mm_hadd_ps(sum1, sum1);

    return _mm_cvtss_f32(sum2);
}

#elif RE_SIMD_SSE
#include <xmmintrin.h>
#include <pmmintrin.h>

RE_INLINE RE_f32 RE_QUAT_DOT_SIMD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    __m128 qa = _mm_loadu_ps(&a.x);
    __m128 qb = _mm_loadu_ps(&b.x);
    __m128 mul = _mm_mul_ps(qa, qb);

    __m128 shuffle = _mm_movehdup_ps(mul);
    __m128 sum = _mm_add_ps(mul, shuffle);
    shuffle = _mm_movehl_ps(shuffle, sum);
    sum = _mm_add_ss(sum, shuffle);

    return _mm_cvtss_f32(sum);
}

#elif RE_SIMD_NEON
#include <arm_neon.h>

RE_INLINE RE_f32 RE_QUAT_DOT_SIMD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    float32x4_t va = vld1q_f32(&a.x);
    float32x4_t vb = vld1q_f32(&b.x);
    float32x4_t mul = vmulq_f32(va, vb);
    float32x2_t sum2 = vadd_f32(vget_low_f32(mul), vget_high_f32(mul));
    float32x2_t sum1 = vpadd_f32(sum2, sum2);
    return vget_lane_f32(sum1, 0);
}

#else
RE_INLINE RE_f32 RE_QUAT_DOT_SIMD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
#endif


/* ================================================================
   SIMD: LENGTH SQUARED
   ================================================================ */

RE_INLINE RE_f32 RE_QUAT_LEN2_SIMD_f32(RE_QUAT_f32 q)
{
    return RE_QUAT_DOT_SIMD_f32(q, q);
}


/* ================================================================
   SIMD: NORMALIZE
   (fast, stable)
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_NORMALIZE_SIMD_f32(RE_QUAT_f32 q)
{
    RE_f32 len2 = RE_QUAT_LEN2_SIMD_f32(q);

    if (len2 <= 0.0f)
        return RE_QUAT_IDENTITY_f32();

    RE_f32 inv = RE_INV_SQRT_FAST_f32(len2);

    RE_QUAT_f32 r = {
        q.x * inv,
        q.y * inv,
        q.z * inv,
        q.w * inv
    };
    return r;
}


/* ================================================================
   SIMD: HAMILTON PRODUCT (q1 * q2)
   (Partial SIMD — best for x,y,z ops)
   ================================================================ */

#if RE_SIMD_AVX || RE_SIMD_SSE

RE_INLINE RE_QUAT_f32 RE_QUAT_MUL_SIMD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    /* load both quats */
    __m128 qa = _mm_loadu_ps(&a.x);
    __m128 qb = _mm_loadu_ps(&b.x);

    /* Shuffle masks */
    const __m128 sign_mask_x = _mm_set_ps(-1, 1, 1, 1);
    const __m128 sign_mask_y = _mm_set_ps( 1,-1, 1, 1);
    const __m128 sign_mask_z = _mm_set_ps( 1, 1,-1, 1);

    /* compute */
    __m128 bx = _mm_shuffle_ps(qb, qb, _MM_SHUFFLE(0,0,0,0));
    __m128 by = _mm_shuffle_ps(qb, qb, _MM_SHUFFLE(1,1,1,1));
    __m128 bz = _mm_shuffle_ps(qb, qb, _MM_SHUFFLE(2,2,2,2));
    __m128 bw = _mm_shuffle_ps(qb, qb, _MM_SHUFFLE(3,3,3,3));

    __m128 rx = _mm_mul_ps(qa, bx);
    __m128 ry = _mm_mul_ps(qa, by);
    __m128 rz = _mm_mul_ps(qa, bz);
    __m128 rw = _mm_mul_ps(qa, bw);

    rx = _mm_mul_ps(rx, sign_mask_x);
    ry = _mm_mul_ps(ry, sign_mask_y);
    rz = _mm_mul_ps(rz, sign_mask_z);

    __m128 out = _mm_add_ps(_mm_add_ps(rx, ry), _mm_add_ps(rz, rw));

    RE_QUAT_f32 r;
    _mm_storeu_ps(&r.x, out);
    return r;
}

#elif RE_SIMD_NEON

RE_INLINE RE_QUAT_f32 RE_QUAT_MUL_SIMD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    float32x4_t qa = vld1q_f32(&a.x);
    float32x4_t qb = vld1q_f32(&b.x);

    float32x4_t bx = vdupq_n_f32(b.x);
    float32x4_t by = vdupq_n_f32(b.y);
    float32x4_t bz = vdupq_n_f32(b.z);
    float32x4_t bw = vdupq_n_f32(b.w);

    float32x4_t rx = vmulq_f32(qa, bx);
    float32x4_t ry = vmulq_f32(qa, by);
    float32x4_t rz = vmulq_f32(qa, bz);
    float32x4_t rw = vmulq_f32(qa, bw);

    /* sign patterns */
    const float32x4_t sx = { 1, 1, 1,-1};
    const float32x4_t sy = { 1, 1,-1, 1};
    const float32x4_t sz = { 1,-1, 1, 1};

    rx = vmulq_f32(rx, sx);
    ry = vmulq_f32(ry, sy);
    rz = vmulq_f32(rz, sz);

    float32x4_t out = vaddq_f32(vaddq_f32(rx, ry), vaddq_f32(rz, rw));

    RE_QUAT_f32 r;
    vst1q_f32(&r.x, out);
    return r;
}

#else

RE_INLINE RE_QUAT_f32 RE_QUAT_MUL_SIMD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    return RE_QUAT_MUL_f32(a, b);
}

#endif


/* ================================================================
   SIMD ROTATE VECTOR (v' = q * v * q^-1)
   ================================================================ */

RE_INLINE RE_V3_f32 RE_QUAT_ROTATE_VEC3_SIMD_f32(RE_QUAT_f32 q, RE_V3_f32 v)
{
    RE_QUAT_f32 pure = { v.x, v.y, v.z, 0.f };

    RE_QUAT_f32 qn = RE_QUAT_NORMALIZE_SIMD_f32(q);

    RE_QUAT_f32 res = RE_QUAT_MUL_SIMD_f32(
        RE_QUAT_MUL_SIMD_f32(qn, pure),
        (RE_QUAT_f32){ -qn.x, -qn.y, -qn.z, qn.w }
    );

    RE_V3_f32 r = { res.x, res.y, res.z };
    return r;
}

RE_INLINE RE_QUAT_f32 RE_QUAT_CONJUGATE_f32(RE_QUAT_f32 q)
{
    RE_QUAT_f32 r = { -q.x, -q.y, -q.z, q.w };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_CONJUGATE_f64(RE_QUAT_f64 q)
{
    RE_QUAT_f64 r = { -q.x, -q.y, -q.z, q.w };
    return r;
}

/* --------------------------
   Inverse q⁻¹ = q* / |q|²
   -------------------------- */
RE_INLINE RE_QUAT_f32 RE_QUAT_INVERSE_f32(RE_QUAT_f32 q)
{
    RE_f32 len2 = RE_QUAT_LEN2_SIMD_f32(q);
    if (len2 <= 0.0f) return RE_QUAT_IDENTITY_f32();
    RE_f32 inv = 1.0f / len2;

    RE_QUAT_f32 r = { -q.x * inv, -q.y * inv, -q.z * inv, q.w * inv };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_INVERSE_f64(RE_QUAT_f64 q)
{
    RE_f64 len2 = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    if (len2 <= 0.0) return RE_QUAT_IDENTITY_f64();
    RE_f64 inv = 1.0 / len2;

    RE_QUAT_f64 r = { -q.x * inv, -q.y * inv, -q.z * inv, q.w * inv };
    return r;
}


/* ============================================================================
   AXIS ROTATION
   ============================================================================ */

RE_INLINE RE_QUAT_f32
RE_QUAT_ROTATE_AXIS_f32(RE_V3_f32 axis, RE_f32 angle_rad)
{
    RE_V3_f32 n = RE_V3_NORMALIZE_f32(axis);
    RE_f32 s = RE_SIN_f32(angle_rad * 0.5f);

    RE_QUAT_f32 q = { n.x*s, n.y*s, n.z*s, RE_COS_f32(angle_rad * 0.5f) };
    return q;
}

/* ============================================================================
   EULER → QUATERNION (XYZ order — standard engine order)
   ============================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_FROM_EULER_f32(RE_V3_f32 e)
{
    RE_f32 cx = RE_COS_f32(e.x * 0.5f), sx = RE_SIN_f32(e.x * 0.5f);
    RE_f32 cy = RE_COS_f32(e.y * 0.5f), sy = RE_SIN_f32(e.y * 0.5f);
    RE_f32 cz = RE_COS_f32(e.z * 0.5f), sz = RE_SIN_f32(e.z * 0.5f);

    RE_QUAT_f32 q;

    q.w = cx*cy*cz + sx*sy*sz;
    q.x = sx*cy*cz - cx*sy*sz;
    q.y = cx*sy*cz + sx*cy*sz;
    q.z = cx*cy*sz - sx*sy*cz;

    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_FROM_EULER_f64(RE_V3_f64 e)
{
    RE_f64 cx = RE_COS_f32(e.x * 0.5), sx = RE_SIN_f32(e.x * 0.5);
    RE_f64 cy = RE_COS_f32(e.y * 0.5), sy = RE_SIN_f32(e.y * 0.5);
    RE_f64 cz = RE_COS_f32(e.z * 0.5), sz = RE_SIN_f32(e.z * 0.5);

    RE_QUAT_f64 q;

    q.w = cx*cy*cz + sx*sy*sz;
    q.x = sx*cy*cz - cx*sy*sz;
    q.y = cx*sy*cz + sx*cy*sz;
    q.z = cx*cy*sz - sx*sy*cz;

    return q;
}


/* ============================================================================
   QUATERNION → EULER
   ============================================================================ */

RE_INLINE RE_V3_f32 RE_QUAT_TO_EULER_f32(RE_QUAT_f32 q)
{
    RE_V3_f32 e;

    // Yaw (Y)
    e.y = RE_ATAN2_f32(2*(q.w*q.y + q.x*q.z),
                 1 - 2*(q.y*q.y + q.x*q.x));

    // Pitch (X)
    RE_f32 s = 2*(q.w*q.x - q.z*q.y);
    e.x = (RE_FABS_f32(s) >= 1) ? RE_COPYSIGN_f32(RE_PI_F, s) : RE_ASIN(s);

    // Roll (Z)
    e.z = RE_ATAN2_f32(2*(q.w*q.z + q.y*q.x),
                 1 - 2*(q.z*q.z + q.x*q.x));

    return e;
}

RE_INLINE RE_V3_f64 RE_QUAT_TO_EULER_f64(RE_QUAT_f64 q)
{
    RE_V3_f64 e;

    e.y = RE_ATAN2_f32(2*(q.w*q.y + q.x*q.z),
                1 - 2*(q.y*q.y + q.x*q.x));

    RE_f64 s = 2*(q.w*q.x - q.z*q.y);
    e.x = (RE_FABS_f32(s) >= 1) ? RE_COPYSIGN_f32(RE_PI_F, s) : RE_ACOS(s);

    e.z = RE_ATAN2_f32(2*(q.w*q.z + q.y*q.x),
                1 - 2*(q.z*q.z + q.x*q.x));

    return e;
}


/* ============================================================================
   LERP
   ============================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_LERP_f32(RE_QUAT_f32 a, RE_QUAT_f32 b, RE_f32 t)
{
    RE_QUAT_f32 r = {
        a.x + (b.x - a.x)*t,
        a.y + (b.y - a.y)*t,
        a.z + (b.z - a.z)*t,
        a.w + (b.w - a.w)*t
    };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_LERP_f64(RE_QUAT_f64 a, RE_QUAT_f64 b, RE_f64 t)
{
    RE_QUAT_f64 r = {
        a.x + (b.x - a.x)*t,
        a.y + (b.y - a.y)*t,
        a.z + (b.z - a.z)*t,
        a.w + (b.w - a.w)*t
    };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_SLERP_f64(RE_QUAT_f64 a, RE_QUAT_f64 b, RE_f64 t)
{
    RE_f64 dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;

    if (dot < 0.0) {
        b.x *= -1; b.y *= -1; b.z *= -1; b.w *= -1;
        dot = -dot;
    }

    if (dot > 0.9995)
        return RE_QUAT_INVERSE_f64(RE_QUAT_LERP_f64(a, b, t));

    RE_f64 theta = RE_ACOS(dot);
    RE_f64 s = RE_SIN_f32(theta);

    RE_f64 w1 = RE_SIN_f32((1-t)*theta) / s;
    RE_f64 w2 = RE_SIN_f32(t*theta) / s;

    RE_QUAT_f64 r = {
        a.x*w1 + b.x*w2,
        a.y*w1 + b.y*w2,
        a.z*w1 + b.z*w2,
        a.w*w1 + b.w*w2
    };
    return r;
}


/* ============================================================================
   ROTATE TOWARDS (unity-compatible)
   ============================================================================ */

RE_INLINE RE_QUAT_f32
RE_QUAT_ROTATE_TOWARDS_f32(RE_QUAT_f32 a, RE_QUAT_f32 b, RE_f32 max_rad)
{
    RE_f32 dot = RE_QUAT_DOT_SIMD_f32(a, b);

    if (dot > 0.999999f)
        return b;

    RE_f32 angle = RE_ACOS(dot);

    RE_f32 t = RE_MIN_f32(1.0f, max_rad / angle);

    return RE_QUAT_SLERP_f32(a, b, t);
}


/* ============================================================================
   GET DIRECTIONS (Forward / Right / Up)
   ============================================================================ */

RE_INLINE RE_V3_f32 RE_QUAT_FORWARD_f32(RE_QUAT_f32 q)
{
    return RE_QUAT_ROTATE_VEC3_SIMD_f32(q, (RE_V3_f32){0,0,-1});
}

RE_INLINE RE_V3_f32 RE_QUAT_RIGHT_f32(RE_QUAT_f32 q)
{
    return RE_QUAT_ROTATE_VEC3_SIMD_f32(q, (RE_V3_f32){1,0,0});
}

RE_INLINE RE_V3_f32 RE_QUAT_UP_f32(RE_QUAT_f32 q)
{
    return RE_QUAT_ROTATE_VEC3_SIMD_f32(q, (RE_V3_f32){0,1,0});
}

#endif /* RE_QUAT_H */
