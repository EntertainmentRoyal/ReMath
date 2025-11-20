#ifndef RE_MAT4_SIMD_H
#define RE_MAT4_SIMD_H

#include "re_core.h"

/*
   RE Mat4 SIMD — Header-only, C-compatible

   Layout: column-major (OpenGL / GLM)
   Index:  m[col * 4 + row]
*/

/* ============================================================================
   Scalar fallback (always available)
   ============================================================================ */

RE_INLINE void
RE_M4F32_MUL_SCALAR(RE_f32 out[16], const RE_f32 A[16], const RE_f32 B[16])
{
    for (int c = 0; c < 4; c++)
    {
        RE_f32 b0 = B[c*4 + 0];
        RE_f32 b1 = B[c*4 + 1];
        RE_f32 b2 = B[c*4 + 2];
        RE_f32 b3 = B[c*4 + 3];

        out[c*4 + 0] = A[0] * b0 + A[4] * b1 + A[8]  * b2 + A[12] * b3;
        out[c*4 + 1] = A[1] * b0 + A[5] * b1 + A[9]  * b2 + A[13] * b3;
        out[c*4 + 2] = A[2] * b0 + A[6] * b1 + A[10] * b2 + A[14] * b3;
        out[c*4 + 3] = A[3] * b0 + A[7] * b1 + A[11] * b2 + A[15] * b3;
    }
}

RE_INLINE void
RE_M4F64_MUL_SCALAR(RE_f64 out[16], const RE_f64 A[16], const RE_f64 B[16])
{
    for (int c = 0; c < 4; c++)
    {
        RE_f64 b0 = B[c*4 + 0];
        RE_f64 b1 = B[c*4 + 1];
        RE_f64 b2 = B[c*4 + 2];
        RE_f64 b3 = B[c*4 + 3];

        out[c*4 + 0] = A[0] * b0 + A[4] * b1 + A[8]  * b2 + A[12] * b3;
        out[c*4 + 1] = A[1] * b0 + A[5] * b1 + A[9]  * b2 + A[13] * b3;
        out[c*4 + 2] = A[2] * b0 + A[6] * b1 + A[10] * b2 + A[14] * b3;
        out[c*4 + 3] = A[3] * b0 + A[7] * b1 + A[11] * b2 + A[15] * b3;
    }
}

/* ============================================================================
   SSE version (x86)
   ============================================================================ */
#if defined(__SSE__) || defined(_MSC_VER)
#include <xmmintrin.h>

RE_INLINE void
RE_M4F32_MUL_SSE(RE_f32 out[16], const RE_f32 A[16], const RE_f32 B[16])
{
    __m128 r0 = _mm_loadu_ps(&A[0]);
    __m128 r1 = _mm_loadu_ps(&A[4]);
    __m128 r2 = _mm_loadu_ps(&A[8]);
    __m128 r3 = _mm_loadu_ps(&A[12]);

    for (int col = 0; col < 4; col++)
    {
        __m128 bc0 = _mm_set1_ps(B[col*4 + 0]);
        __m128 bc1 = _mm_set1_ps(B[col*4 + 1]);
        __m128 bc2 = _mm_set1_ps(B[col*4 + 2]);
        __m128 bc3 = _mm_set1_ps(B[col*4 + 3]);

        __m128 acc = _mm_mul_ps(r0, bc0);
        acc = _mm_add_ps(acc, _mm_mul_ps(r1, bc1));
        acc = _mm_add_ps(acc, _mm_mul_ps(r2, bc2));
        acc = _mm_add_ps(acc, _mm_mul_ps(r3, bc3));

        _mm_storeu_ps(&out[col*4], acc);
    }
}
#endif

/* ============================================================================
   AVX version (x86)
   ============================================================================ */
#if defined(__AVX__)
#include <immintrin.h>

RE_INLINE void
RE_M4F32_MUL_AVX(RE_f32 out[16], const RE_f32 A[16], const RE_f32 B[16])
{
    __m256 row01 = _mm256_loadu_ps(&A[0]);   // rows 0 & 1
    __m256 row23 = _mm256_loadu_ps(&A[8]);   // rows 2 & 3

    for (int col = 0; col < 4; col++)
    {
        __m256 b0 = _mm256_broadcast_ss(&B[col*4 + 0]);
        __m256 b1 = _mm256_broadcast_ss(&B[col*4 + 1]);
        __m256 b2 = _mm256_broadcast_ss(&B[col*4 + 2]);
        __m256 b3 = _mm256_broadcast_ss(&B[col*4 + 3]);

        __m256 acc01 = _mm256_mul_ps(row01, b0);
        __m256 acc23 = _mm256_mul_ps(row23, b0);

        acc01 = _mm256_add_ps(acc01, _mm256_mul_ps(row01, b1));
        acc23 = _mm256_add_ps(acc23, _mm256_mul_ps(row23, b1));

        acc01 = _mm256_add_ps(acc01, _mm256_mul_ps(row01, b2));
        acc23 = _mm256_add_ps(acc23, _mm256_mul_ps(row23, b2));

        acc01 = _mm256_add_ps(acc01, _mm256_mul_ps(row01, b3));
        acc23 = _mm256_add_ps(acc23, _mm256_mul_ps(row23, b3));

        _mm256_storeu_ps(&out[col*4 + 0], acc01);
        _mm256_storeu_ps(&out[col*4 + 8], acc23);
    }
}
#endif

/* ============================================================================
   NEON version (ARM)
   ============================================================================ */
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>

RE_INLINE void
RE_M4F32_MUL_NEON(RE_f32 out[16], const RE_f32 A[16], const RE_f32 B[16])
{
    float32x4_t r0 = vld1q_f32(&A[0]);
    float32x4_t r1 = vld1q_f32(&A[4]);
    float32x4_t r2 = vld1q_f32(&A[8]);
    float32x4_t r3 = vld1q_f32(&A[12]);

    for (int col = 0; col < 4; col++)
    {
        float32x4_t acc = vmulq_n_f32(r0, B[col*4 + 0]);
        acc = vmlaq_n_f32(acc, r1, B[col*4 + 1]);
        acc = vmlaq_n_f32(acc, r2, B[col*4 + 2]);
        acc = vmlaq_n_f32(acc, r3, B[col*4 + 3]);

        vst1q_f32(&out[col*4], acc);
    }
}
#endif

/* ============================================================================
   Master selector: choose best SIMD available, else fallback
   ============================================================================ */

RE_INLINE void
RE_M4F32_MUL(RE_f32 out[16], const RE_f32 A[16], const RE_f32 B[16])
{
#if defined(__AVX__)
    RE_M4F32_MUL_AVX(out, A, B);
#elif defined(__SSE__) || defined(_MSC_VER)
    RE_M4F32_MUL_SSE(out, A, B);
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    RE_M4F32_MUL_NEON(out, A, B);
#else
    RE_M4F32_MUL_SCALAR(out, A, B);
#endif
}

/* F64 — scalar only (SIMD double versions require AVX2, optional) */
RE_INLINE void
RE_M4F64_MUL(RE_f64 out[16], const RE_f64 A[16], const RE_f64 B[16])
{
    RE_M4F64_MUL_SCALAR(out, A, B);
}

#endif /* RE_MAT4_SIMD_H */
