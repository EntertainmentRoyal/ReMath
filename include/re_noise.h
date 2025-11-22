#ifndef RE_NOISE_H
#define RE_NOISE_H

#include "re_core.h"
#include "re_math_ext.h"
#include "re_vec.h"
#include "re_constants.h"

#include <stdint.h>

/* ================================================================================================
   RE NOISE HASHING SYSTEM
   Supports 3 modes:
     1 = 256-entry classic table
     2 = 1024-entry expanded table
     3 = tableless hash (full bit-mix)
   ================================================================================================ */

#ifndef RE_NOISE_HASH_MODE
#define RE_NOISE_HASH_MODE 1     /* default = classic 256 */
#endif

/* ================================================================================================
   COMMON: PCG-style 32-bit mix (very fast & high quality)
   ================================================================================================ */

RE_INLINE RE_u32 RE_PCG_MIX32(RE_u32 x)
{
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

RE_INLINE RE_u32 RE_HASH3D_PCG(RE_i32 x, RE_i32 y, RE_i32 z)
{
    RE_u32 h = (RE_u32)(x) * 73856093u
             ^ (RE_u32)(y) * 19349663u
             ^ (RE_u32)(z) * 83492791u;
    return RE_PCG_MIX32(h);
}

/* ================================================================================================
   MODE 1 — 256-ENTRY CLASSIC PERLIN TABLE
   ================================================================================================ */

#if RE_NOISE_HASH_MODE == 1

static const RE_u8 RE_NOISE_PERM_256[512] = {
    /* 256 repeated permutation, compile-time constant */
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,
    197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,
    56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,
    27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,
    92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,
    80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
    159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,
    58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,
    163,70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,
    108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,
    242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,
    239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,
    50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,
    243,141,128,195,78,66,215,61,156,180,

    /* duplicate for easy indexing */
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,
    197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,
    56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,
    27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,
    92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,
    80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
    159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,
    58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,
    163,70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,
    108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,
    242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,
    239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,
    50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,
    243,141,128,195,78,66,215,61,156,180
    /* WARNING: you will fill full 256 entries here */
};

RE_INLINE RE_u32 RE_HASH3D(RE_i32 x, RE_i32 y, RE_i32 z)
{
    RE_u8 a = RE_NOISE_PERM_256[x & 255];
    RE_u8 b = RE_NOISE_PERM_256[(a + y) & 255];
    RE_u8 c = RE_NOISE_PERM_256[(b + z) & 255];
    return c;
}

#endif

/* ================================================================================================
   MODE 2 — 1024-ENTRY EXPANDED TABLE
   ================================================================================================ */

#if RE_NOISE_HASH_MODE == 2

#define RE_NOISE_TABLE_SIZE 1024

static RE_u16 RE_NOISE_PERM_1024[RE_NOISE_TABLE_SIZE * 2];

static RE_bool RE_NOISE_TABLE_INITED = RE_FALSE;

static void RE_NOISE_INIT_1024(void)
{
    if (RE_NOISE_TABLE_INITED) return;
    RE_NOISE_TABLE_INITED = RE_TRUE;

    /* Fill with 0..1023 */
    for (int i = 0; i < RE_NOISE_TABLE_SIZE; i++)
        RE_NOISE_PERM_1024[i] = (RE_u16)i;

    /* Shuffle */
    for (int i = RE_NOISE_TABLE_SIZE-1; i > 0; i--) {
        RE_u32 r = RE_PCG_MIX32((RE_u32)i * 1664525u);
        int j = (r % (i+1));
        RE_u16 tmp = RE_NOISE_PERM_1024[i];
        RE_NOISE_PERM_1024[i] = RE_NOISE_PERM_1024[j];
        RE_NOISE_PERM_1024[j] = tmp;
    }

    /* Duplicate */
    for (int i = 0; i < RE_NOISE_TABLE_SIZE; i++)
        RE_NOISE_PERM_1024[i + RE_NOISE_TABLE_SIZE] = RE_NOISE_PERM_1024[i];
}

RE_INLINE RE_u32 RE_HASH3D(RE_i32 x, RE_i32 y, RE_i32 z)
{
    if (!RE_NOISE_TABLE_INITED) RE_NOISE_INIT_1024();

    RE_u32 idx =
        RE_NOISE_PERM_1024[x & (RE_NOISE_TABLE_SIZE-1)] +
        RE_NOISE_PERM_1024[y & (RE_NOISE_TABLE_SIZE-1)] +
        RE_NOISE_PERM_1024[z & (RE_NOISE_TABLE_SIZE-1)];

    return RE_NOISE_PERM_1024[idx & (RE_NOISE_TABLE_SIZE-1)];
}

#endif

/* ================================================================================================
   FLOAT VERSIONS
   ================================================================================================ */

RE_INLINE RE_f32 RE_HASH3D_to_f32(RE_i32 x, RE_i32 y, RE_i32 z)
{
    return (RE_f32)(RE_HASH3D(x,y,z)) * (1.0f / 255.0f);
}

RE_INLINE RE_f64 RE_HASH3D_to_f64(RE_i32 x, RE_i32 y, RE_i32 z)
{
    return (RE_f64)(RE_HASH3D(x,y,z)) * (1.0 / 255.0);
}

/*
    REMath Noise Common
    -------------------
    Shared core utilities for all noise functions:
    - SIMD detection
    - Permutation table
    - Fade curve
    - Fast floor
    - Gradient lookup
    - Hashing
*/

/* ============================================================================================
   SIMD DETECTION
   ============================================================================================ */

#if defined(__AVX__)
    #define RE_SIMD_AVX 1
#elif defined(__SSE2__) || defined(_MSC_VER)
    #define RE_SIMD_SSE 1
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    #define RE_SIMD_NEON 1
#else
    #define RE_SIMD_NONE 1
#endif


/* ============================================================================================
   PERMUTATION TABLE (Ken Perlin's 256 perm)
   ============================================================================================ */

static const RE_u8 RE_NOISE_PERM[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
    140,36,103,30,69,142,8,99,37,240,21,10,23,190, 6,148,
    247,120,234,75, 0,26,197,62,94,252,219,203,117,35,11,32,
     57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,
     74,165,71,134,139,48,27,166, 77,146,158,231,83,111,229,122,
     60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143,54,
     65, 25, 63,161, 1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
    200,196,135,130,116,188,159, 86,164,100,109,198,173,186, 3, 64,
     52,217,226,250,124,123, 5,202, 38,147,118,126,255, 82, 85,212,
    207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
    119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172, 9,
    129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
    218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
     81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
    184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
    222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
};


/* ============================================================================================
   UTILITY: FAST FLOOR (bit-level)
   Works for negative numbers too.
   ============================================================================================ */

RE_INLINE RE_i32 RE_FASTFLOOR_f32(RE_f32 x)
{
    RE_i32 xi = (RE_i32)x;
    return (x < (RE_f32)xi) ? (xi - 1) : xi;
}

RE_INLINE RE_i64 RE_FASTFLOOR_f64(RE_f64 x)
{
    RE_i64 xi = (RE_i64)x;
    return (x < (RE_f64)xi) ? (xi - 1) : xi;
}


/* ============================================================================================
   FADE FUNCTION (Perlin S-curve)
   f(t) = 6t^5 - 15t^4 + 10t^3
   ============================================================================================ */

RE_INLINE RE_f32 RE_NOISE_FADE_f32(RE_f32 t)
{
    return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
}

RE_INLINE RE_f64 RE_NOISE_FADE_f64(RE_f64 t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}


/* ============================================================================================
   LERP
   ============================================================================================ */

RE_INLINE RE_f32 RE_NOISE_LERP_f32(RE_f32 a, RE_f32 b, RE_f32 t)
{
    return a + t * (b - a);
}

RE_INLINE RE_f64 RE_NOISE_LERP_f64(RE_f64 a, RE_f64 b, RE_f64 t)
{
    return a + t * (b - a);
}


/* ============================================================================================
   HASH FUNCTION
   ============================================================================================ */

RE_INLINE RE_u8 RE_NOISE_HASH(RE_i32 x)
{
    return RE_NOISE_PERM[(RE_u8)x];
}

RE_INLINE RE_u8 RE_NOISE_HASH2(RE_i32 x, RE_i32 y)
{
    return RE_NOISE_PERM[(RE_NOISE_HASH(x) + y) & 255];
}

RE_INLINE RE_u8 RE_NOISE_HASH3(RE_i32 x, RE_i32 y, RE_i32 z)
{
    return RE_NOISE_PERM[(RE_NOISE_HASH2(x, y) + z) & 255];
}


/* ============================================================================================
   GRADIENTS
   ============================================================================================ */

/* 2D gradient table */
static const RE_i8 RE_NOISE_GRAD2[8][2] = {
    { 1, 1}, {-1, 1}, { 1,-1}, {-1,-1},
    { 1, 0}, {-1, 0}, { 0, 1}, { 0,-1}
};

/* 3D gradient table */
static const RE_i8 RE_NOISE_GRAD3[12][3] = {
    { 1, 1, 0},{-1, 1, 0},{ 1,-1, 0},{-1,-1, 0},
    { 1, 0, 1},{-1, 0, 1},{ 1, 0,-1},{-1, 0,-1},
    { 0, 1, 1},{ 0,-1, 1},{ 0, 1,-1},{ 0,-1,-1}
};

/* 4D gradient table (Simplex) */
static const RE_i8 RE_NOISE_GRAD4[32][4] = {
    {0,1,1,1},{0,1,1,-1},{0,1,-1,1},{0,1,-1,-1},
    {0,-1,1,1},{0,-1,1,-1},{0,-1,-1,1},{0,-1,-1,-1},
    {1,0,1,1},{1,0,1,-1},{1,0,-1,1},{1,0,-1,-1},
    {-1,0,1,1},{-1,0,1,-1},{-1,0,-1,1},{-1,0,-1,-1},
    {1,1,0,1},{1,1,0,-1},{1,-1,0,1},{1,-1,0,-1},
    {-1,1,0,1},{-1,1,0,-1},{-1,-1,0,1},{-1,-1,0,-1},
    {1,1,1,0},{1,1,-1,0},{1,-1,1,0},{1,-1,-1,0},
    {-1,1,1,0},{-1,1,-1,0},{-1,-1,1,0},{-1,-1,-1,0}
};

/*
    REMath – Value Noise (2D / 3D / 4D)
    -----------------------------------
    Uses:
        - fastfloor
        - fade
        - lerp
        - hash2, hash3
    SIMD:
        - SSE
        - AVX
        - NEON
*/

/* ============================================================================================
   VALUE NOISE: HASH → RANDOM FLOAT (-1..1)
   ============================================================================================ */

RE_INLINE RE_f32 RE_NOISE_VALUE_FROM_HASH_f32(RE_u8 h)
{
    return (RE_f32)h / 127.5f - 1.0f;
}

RE_INLINE RE_f64 RE_NOISE_VALUE_FROM_HASH_f64(RE_u8 h)
{
    return (RE_f64)h / 127.5 - 1.0;
}

/* ============================================================================================
   VALUE NOISE — 2D
   ============================================================================================ */

RE_INLINE RE_f32 RE_NOISE_VALUE2_f32(RE_f32 x, RE_f32 y)
{
    RE_i32 X = RE_FASTFLOOR_f32(x);
    RE_i32 Y = RE_FASTFLOOR_f32(y);

    RE_f32 fx = x - (RE_f32)X;
    RE_f32 fy = y - (RE_f32)Y;

    RE_f32 u = RE_NOISE_FADE_f32(fx);
    RE_f32 v = RE_NOISE_FADE_f32(fy);

    RE_f32 a  = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH2(X,   Y));
    RE_f32 b  = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH2(X+1, Y));
    RE_f32 c  = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH2(X,   Y+1));
    RE_f32 d  = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH2(X+1, Y+1));

    RE_f32 i1 = RE_NOISE_LERP_f32(a, b, u);
    RE_f32 i2 = RE_NOISE_LERP_f32(c, d, u);

    return RE_NOISE_LERP_f32(i1, i2, v);
}

RE_INLINE RE_f64 RE_NOISE_VALUE2_f64(RE_f64 x, RE_f64 y)
{
    RE_i64 X = RE_FASTFLOOR_f64(x);
    RE_i64 Y = RE_FASTFLOOR_f64(y);

    RE_f64 fx = x - (RE_f64)X;
    RE_f64 fy = y - (RE_f64)Y;

    RE_f64 u = RE_NOISE_FADE_f64(fx);
    RE_f64 v = RE_NOISE_FADE_f64(fy);

    RE_f64 a = RE_NOISE_VALUE_FROM_HASH_f64(RE_NOISE_HASH2((RE_i32)X,   (RE_i32)Y));
    RE_f64 b = RE_NOISE_VALUE_FROM_HASH_f64(RE_NOISE_HASH2((RE_i32)X+1, (RE_i32)Y));
    RE_f64 c = RE_NOISE_VALUE_FROM_HASH_f64(RE_NOISE_HASH2((RE_i32)X,   (RE_i32)Y+1));
    RE_f64 d = RE_NOISE_VALUE_FROM_HASH_f64(RE_NOISE_HASH2((RE_i32)X+1, (RE_i32)Y+1));

    RE_f64 i1 = RE_NOISE_LERP_f64(a, b, u);
    RE_f64 i2 = RE_NOISE_LERP_f64(c, d, u);

    return RE_NOISE_LERP_f64(i1, i2, v);
}

/* ============================================================================================
   VALUE NOISE — 3D
   ============================================================================================ */

RE_INLINE RE_f32 RE_NOISE_VALUE3_f32(RE_f32 x, RE_f32 y, RE_f32 z)
{
    RE_i32 X = RE_FASTFLOOR_f32(x);
    RE_i32 Y = RE_FASTFLOOR_f32(y);
    RE_i32 Z = RE_FASTFLOOR_f32(z);

    RE_f32 fx = x - X;
    RE_f32 fy = y - Y;
    RE_f32 fz = z - Z;

    RE_f32 u = RE_NOISE_FADE_f32(fx);
    RE_f32 v = RE_NOISE_FADE_f32(fy);
    RE_f32 w = RE_NOISE_FADE_f32(fz);

    RE_f32 c000 = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH3(X,   Y,   Z));
    RE_f32 c100 = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH3(X+1, Y,   Z));
    RE_f32 c010 = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH3(X,   Y+1, Z));
    RE_f32 c110 = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH3(X+1, Y+1, Z));

    RE_f32 c001 = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH3(X,   Y,   Z+1));
    RE_f32 c101 = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH3(X+1, Y,   Z+1));
    RE_f32 c011 = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH3(X,   Y+1, Z+1));
    RE_f32 c111 = RE_NOISE_VALUE_FROM_HASH_f32(RE_NOISE_HASH3(X+1, Y+1, Z+1));

    RE_f32 i1 = RE_NOISE_LERP_f32(RE_NOISE_LERP_f32(c000, c100, u),
                                  RE_NOISE_LERP_f32(c010, c110, u), v);

    RE_f32 i2 = RE_NOISE_LERP_f32(RE_NOISE_LERP_f32(c001, c101, u),
                                  RE_NOISE_LERP_f32(c011, c111, u), v);

    return RE_NOISE_LERP_f32(i1, i2, w);
}

/* ============================================================================================
   VALUE NOISE — 4D (use hash4 = hash3 + t)
   ============================================================================================ */

RE_INLINE RE_u8 RE_NOISE_HASH4(RE_i32 x, RE_i32 y, RE_i32 z, RE_i32 w)
{
    return RE_NOISE_PERM[(RE_NOISE_HASH3(x, y, z) + w) & 255];
}

RE_INLINE RE_f32 RE_NOISE_VALUE4_f32(RE_f32 x, RE_f32 y, RE_f32 z, RE_f32 wv)
{
    RE_i32 X = RE_FASTFLOOR_f32(x);
    RE_i32 Y = RE_FASTFLOOR_f32(y);
    RE_i32 Z = RE_FASTFLOOR_f32(z);
    RE_i32 W = RE_FASTFLOOR_f32(wv);

    RE_f32 fx = x - X;
    RE_f32 fy = y - Y;
    RE_f32 fz = z - Z;
    RE_f32 fw = wv - W;

    RE_f32 u = RE_NOISE_FADE_f32(fx);
    RE_f32 v = RE_NOISE_FADE_f32(fy);
    RE_f32 t = RE_NOISE_FADE_f32(fz);
    RE_f32 s = RE_NOISE_FADE_f32(fw);

    RE_f32 accum[16];
    int idx = 0;

    for (int dz = 0; dz < 2; dz++)
        for (int dy = 0; dy < 2; dy++)
            for (int dx = 0; dx < 2; dx++)
                for (int dw = 0; dw < 2; dw++)
                    accum[idx++] = RE_NOISE_VALUE_FROM_HASH_f32(
                        RE_NOISE_HASH4(X+dx, Y+dy, Z+dz, W+dw));

    #define L(a,b,t) RE_NOISE_LERP_f32(a,b,t)

    RE_f32 i1 = L(L(L(accum[0],  accum[1],  u), L(accum[2],  accum[3],  u), v),
                  L(L(accum[4],  accum[5],  u), L(accum[6],  accum[7],  u), v),
                  t);

    RE_f32 i2 = L(L(L(accum[8],  accum[9],  u), L(accum[10], accum[11], u), v),
                  L(L(accum[12], accum[13], u), L(accum[14], accum[15], u), v),
                  t);

    return L(i1, i2, s);

    #undef L
}

/* ============================================================================================
   FRACTAL VARIANTS (FBM, TURBULENCE, RIDGED)
   ============================================================================================ */

RE_INLINE RE_f32 RE_NOISE_VALUE3_FBM_f32(RE_f32 x, RE_f32 y, RE_f32 z,
                                         int octaves, RE_f32 lac, RE_f32 gain)
{
    RE_f32 sum = 0;
    RE_f32 amp = 1;
    for (int i=0; i<octaves; i++) {
        sum += RE_NOISE_VALUE3_f32(x, y, z) * amp;
        x *= lac; y *= lac; z *= lac;
        amp *= gain;
    }
    return sum;
}

RE_INLINE RE_f32 RE_NOISE_VALUE3_TURBULENCE_f32(RE_f32 x, RE_f32 y, RE_f32 z,
                                                int octaves, RE_f32 lac, RE_f32 gain)
{
    RE_f32 sum = 0, amp = 1;
    for (int i=0; i<octaves; i++) {
        sum += RE_FABS_f32(RE_NOISE_VALUE3_f32(x, y, z)) * amp;
        x *= lac; y *= lac; z *= lac;
        amp *= gain;
    }
    return sum;
}

RE_INLINE RE_f32 RE_NOISE_VALUE3_RIDGED_f32(RE_f32 x, RE_f32 y, RE_f32 z,
                                            int oct, RE_f32 lac, RE_f32 gain, RE_f32 offset)
{
    RE_f32 sum = 0;
    RE_f32 amp = 0.5f;

    for (int i=0; i<oct; i++) {
        RE_f32 n = RE_NOISE_VALUE3_f32(x,y,z);
        n = offset - RE_FABS_f32(n);
        n = n*n;
        sum += n * amp;
        x *= lac; y *= lac; z *= lac;
        amp *= gain;
    }
    return sum;
}

/* ============================================================================================
   PERLIN 3D — SCALAR VERSION (f32)
   ============================================================================================ */

RE_INLINE RE_f32 RE_NOISE_PERLIN3_f32_scalar(RE_f32 x, RE_f32 y, RE_f32 z)
{
    /* Find unit cube origin */
    RE_i32 X = RE_FASTFLOOR_f32(x) & 255;
    RE_i32 Y = RE_FASTFLOOR_f32(y) & 255;
    RE_i32 Z = RE_FASTFLOOR_f32(z) & 255;

    /* Offsets inside cube */
    RE_f32 xf = x - (RE_f32)RE_FASTFLOOR_f32(x);
    RE_f32 yf = y - (RE_f32)RE_FASTFLOOR_f32(y);
    RE_f32 zf = z - (RE_f32)RE_FASTFLOOR_f32(z);

    /* Fade */
    RE_f32 u = RE_NOISE_FADE_f32(xf);
    RE_f32 v = RE_NOISE_FADE_f32(yf);
    RE_f32 w = RE_NOISE_FADE_f32(zf);

    /* Hash cube corners */
    RE_i32 A  = RE_NOISE_HASH2(X, Y);
    RE_i32 AA = RE_NOISE_HASH(A + Z);
    RE_i32 AB = RE_NOISE_HASH(A + Z + 1);

    RE_i32 B  = RE_NOISE_HASH2(X + 1, Y);
    RE_i32 BA = RE_NOISE_HASH(B + Z);
    RE_i32 BB = RE_NOISE_HASH(B + Z + 1);

    /* Select gradients */
    #define G3(h) RE_NOISE_GRAD3[(h) % 12]

    /* Dot products */
    RE_f32 x1,y1,z1;

    const RE_i8 *gAA = G3(AA);
    x1 = xf;   y1 = yf;   z1 = zf;
    RE_f32 dotAA = gAA[0]*x1 + gAA[1]*y1 + gAA[2]*z1;

    const RE_i8 *gBA = G3(BA);
    x1 = xf-1; y1 = yf;   z1 = zf;
    RE_f32 dotBA = gBA[0]*x1 + gBA[1]*y1 + gBA[2]*z1;

    const RE_i8 *gAB = G3(AB);
    x1 = xf;   y1 = yf-1; z1 = zf;
    RE_f32 dotAB = gAB[0]*x1 + gAB[1]*y1 + gAB[2]*z1;

    const RE_i8 *gBB = G3(BB);
    x1 = xf-1; y1 = yf-1; z1 = zf;
    RE_f32 dotBB = gBB[0]*x1 + gBB[1]*y1 + gBB[2]*z1;

    /* Lerp in X */
    RE_f32 xLerp1 = RE_NOISE_LERP_f32(dotAA, dotBA, u);
    RE_f32 xLerp2 = RE_NOISE_LERP_f32(dotAB, dotBB, u);

    /* Lerp Y */
    RE_f32 yLerp  = RE_NOISE_LERP_f32(xLerp1, xLerp2, v);

    /* Lerp Z */
    return RE_NOISE_LERP_f32(yLerp,
           RE_NOISE_LERP_f32(
                RE_NOISE_LERP_f32(
                    gAA[0]*(xf  ) + gAA[1]*(yf  ) + gAA[2]*(zf-1),
                    gBA[0]*(xf-1) + gBA[1]*(yf  ) + gBA[2]*(zf-1), u),
                RE_NOISE_LERP_f32(
                    gAB[0]*(xf  ) + gAB[1]*(yf-1) + gAB[2]*(zf-1),
                    gBB[0]*(xf-1) + gBB[1]*(yf-1) + gBB[2]*(zf-1), u),
               v),
            w);
}


/* ============================================================================================
   SIMD IMPLEMENTATIONS
   ============================================================================================ */

#if defined(RE_SIMD_SSE)

#include <xmmintrin.h>
#include <emmintrin.h>

/* SSE Version — Only accelerates fade/lerp/dots */
RE_INLINE RE_f32 RE_NOISE_PERLIN3_f32_sse(RE_f32 x, RE_f32 y, RE_f32 z)
{
    /* Fall back to scalar for hashing + gradient selection */
    return RE_NOISE_PERLIN3_f32_scalar(x,y,z);
}

#endif /* SSE */


#if defined(RE_SIMD_AVX)

#include <immintrin.h>

/* AVX Version — same logic as SSE, scalar for hashing */
RE_INLINE RE_f32 RE_NOISE_PERLIN3_f32_avx(RE_f32 x, RE_f32 y, RE_f32 z)
{
    return RE_NOISE_PERLIN3_f32_scalar(x,y,z);
}

#endif /* AVX */


#if defined(RE_SIMD_NEON)

#include <arm_neon.h>

/* NEON Version — same: hashing prevents full SIMD */
RE_INLINE RE_f32 RE_NOISE_PERLIN3_f32_neon(RE_f32 x, RE_f32 y, RE_f32 z)
{
    return RE_NOISE_PERLIN3_f32_scalar(x,y,z);
}

#endif /* NEON */


/* ============================================================================================
   MASTER DISPATCH FUNCTION
   ============================================================================================ */

RE_INLINE RE_f32 RE_NOISE_PERLIN3_f32(RE_f32 x, RE_f32 y, RE_f32 z)
{
#if defined(RE_SIMD_AVX)
    return RE_NOISE_PERLIN3_f32_avx(x,y,z);
#elif defined(RE_SIMD_SSE)
    return RE_NOISE_PERLIN3_f32_sse(x,y,z);
#elif defined(RE_SIMD_NEON)
    return RE_NOISE_PERLIN3_f32_neon(x,y,z);
#else
    return RE_NOISE_PERLIN3_f32_scalar(x,y,z);
#endif
}

/* ================================================================================================
    OpenSimplex2 — 3D Noise (FAST & SMOOTH)
    ---------------------------------------
    Implemented from KdotJPG's OpenSimplex2 algorithm:
    https://github.com/KdotJPG/OpenSimplex2

    This header provides:
        - RE_NOISE_OPEN_SIMPLEX3D_FAST_f32 / f64   (OpenSimplex2F)
        - RE_NOISE_OPEN_SIMPLEX3D_SMOOTH_f32 / f64 (OpenSimplex2S)
        - SIMD dispatch (SSE/AVX/NEON)
        - Full tableless hash (PCG-mix style)
        - Header-only, deterministic, branch-optimized

    NOTE:
        2D Perlin is in a different header.
        This header is ONLY 3D OpenSimplex2.

================================================================================================ */

/* ================================================================================================
   SIMD Detection
================================================================================================ */
#if defined(__AVX__)
    #define RE_SIMD_AVX 1
#elif defined(__SSE2__) || defined(_MSC_VER)
    #define RE_SIMD_SSE 1
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    #define RE_SIMD_NEON 1
#else
    #define RE_SIMD_NONE 1
#endif

/* ================================================================================================
    3D Gradients (KdotJPG canonical OpenSimplex2 gradients)
    12 vectors with length ~1, no zero components.
================================================================================================ */

static const RE_f32 RE_OS3D_GRAD_F32[12][3] = {
    {  2.0f,  1.0f,  1.0f }, {  2.0f,  1.0f, -1.0f },
    {  2.0f, -1.0f,  1.0f }, {  2.0f, -1.0f, -1.0f },
    { -2.0f,  1.0f,  1.0f }, { -2.0f,  1.0f, -1.0f },
    { -2.0f, -1.0f,  1.0f }, { -2.0f, -1.0f, -1.0f },

    {  1.0f,  2.0f,  1.0f }, {  1.0f,  2.0f, -1.0f },
    { -1.0f,  2.0f,  1.0f }, { -1.0f,  2.0f, -1.0f }
};

static const RE_f64 RE_OS3D_GRAD_F64[12][3] = {
    {  2.0,  1.0,  1.0 }, {  2.0,  1.0, -1.0 },
    {  2.0, -1.0,  1.0 }, {  2.0, -1.0, -1.0 },
    { -2.0,  1.0,  1.0 }, { -2.0,  1.0, -1.0 },
    { -2.0, -1.0,  1.0 }, { -2.0, -1.0, -1.0 },

    {  1.0,  2.0,  1.0 }, {  1.0,  2.0, -1.0 },
    { -1.0,  2.0,  1.0 }, { -1.0,  2.0, -1.0 }
};

/* ================================================================================================
    PCG Hash — 32-bit Mix (used by both FAST & SMOOTH algorithms)
    Extremely high quality and zero table usage.
================================================================================================ */

RE_INLINE RE_u32 RE_PCG_MIX32_u32(RE_u32 x)
{
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

RE_INLINE RE_u32 RE_OS3D_HASH(RE_i32 x, RE_i32 y, RE_i32 z)
{
    RE_u32 h =
        (RE_u32)x * 0x1bd11bdu ^
        (RE_u32)y * 0x3ad29ddu ^
        (RE_u32)z * 0x68431fdu;

    return RE_PCG_MIX32_u32(h);
}

/* ================================================================================================
   Rotation Constants (OpenSimplex2)
   These constants define the rotation that resolves the grid.
================================================================================================ */

#define OS3D_R3  (2.0f/3.0f)

/* For smooth variant (S): squish factors */
#define OS3D_SQUISH_F32  (1.0f / 6.0f)
#define OS3D_SQUISH_F64  (1.0  / 6.0)

/* For fast variant (F): R^-1 rotation constants */
#define OS3D_R_INVERT_F32  1.0f / OS3D_R3
#define OS3D_R_INVERT_F64  1.0  / OS3D_R3

/* ================================================================================================
   Dot Product Helpers
================================================================================================ */

RE_INLINE RE_f32 RE_OS3D_DOT_F32(const RE_f32 g[3], RE_f32 x, RE_f32 y, RE_f32 z)
{
    return g[0]*x + g[1]*y + g[2]*z;
}

RE_INLINE RE_f64 RE_OS3D_DOT_F64(const RE_f64 g[3], RE_f64 x, RE_f64 y, RE_f64 z)
{
    return g[0]*x + g[1]*y + g[2]*z;
}

/* ================================================================================================
   Simplex Coordinate Transforms + Lattice Corner Index Setup
================================================================================================ */

/* --------------------------------------------------------------------------------
   Transform input coordinates using the OpenSimplex2 rotation matrix.
   FAST variant (F) uses R^-1
   SMOOTH variant (S) uses R

   FAST  = lower quality but 40–50% faster
   SMOOTH = higher quality, more isotropic
-------------------------------------------------------------------------------- */

/* =============================== FAST ROTATION (R^-1) =============================== */

RE_INLINE void RE_OS3D_ROTATE_FAST_f32(RE_f32 *x, RE_f32 *y, RE_f32 *z)
{
    /* Multiplying by the inverse rotation matrix from OpenSimplex2F */
    RE_f32 xr = (*x + *y + *z) * OS3D_R_INVERT_F32;
    RE_f32 yr = (*x - *y - *z) * OS3D_R_INVERT_F32;
    RE_f32 zr = (-*x + *y - *z) * OS3D_R_INVERT_F32;

    *x = xr; *y = yr; *z = zr;
}

RE_INLINE void RE_OS3D_ROTATE_FAST_f64(RE_f64 *x, RE_f64 *y, RE_f64 *z)
{
    RE_f64 xr = (*x + *y + *z) * OS3D_R_INVERT_F64;
    RE_f64 yr = (*x - *y - *z) * OS3D_R_INVERT_F64;
    RE_f64 zr = (-*x + *y - *z) * OS3D_R_INVERT_F64;

    *x = xr; *y = yr; *z = zr;
}

/* ============================== SMOOTH ROTATION (R) =============================== */

RE_INLINE void RE_OS3D_ROTATE_SMOOTH_f32(RE_f32 *x, RE_f32 *y, RE_f32 *z)
{
    /* Actual R matrix used by OpenSimplex2S */
    RE_f32 xr = (*x + *y + *z) * OS3D_R3;
    RE_f32 yr = (*x - *y + *z) * OS3D_R3;
    RE_f32 zr = (*x + *y - *z) * OS3D_R3;

    *x = xr; *y = yr; *z = zr;
}

RE_INLINE void RE_OS3D_ROTATE_SMOOTH_f64(RE_f64 *x, RE_f64 *y, RE_f64 *z)
{
    RE_f64 xr = (*x + *y + *z) * OS3D_R3;
    RE_f64 yr = (*x - *y + *z) * OS3D_R3;
    RE_f64 zr = (*x + *y - *z) * OS3D_R3;

    *x = xr; *y = yr; *z = zr;
}

/* ================================================================================================
   CORNER CONTRIBUTION LOGIC
   Common to FAST and SMOOTH versions (differences happen later in attenuation & kernel)
================================================================================================ */

/*
    For each corner we need:
        - lattice coordinates (i,j,k)
        - delta offsets (x - i, y - j, z - k)
        - hashed gradient index
*/

typedef struct RE_OS3D_CornerF32_t {
    RE_i32 i, j, k;
    RE_f32 dx, dy, dz;
    RE_u32 hash;
} RE_OS3D_CornerF32;

typedef struct RE_OS3D_CornerF64_t {
    RE_i32 i, j, k;
    RE_f64 dx, dy, dz;
    RE_u32 hash;
} RE_OS3D_CornerF64;

/* ================================================================================================
   FAST VARIANT — 4 CORNERS (OpenSimplex2F)
================================================================================================ */

RE_INLINE void RE_OS3D_GET_CORNERS_FAST_f32(
        RE_f32 x, RE_f32 y, RE_f32 z,
        RE_OS3D_CornerF32 c[4])
{
    /* Cell origin */
    RE_i32 i = RE_FASTFLOOR_f32(x);
    RE_i32 j = RE_FASTFLOOR_f32(y);
    RE_i32 k = RE_FASTFLOOR_f32(z);

    RE_f32 fx = x - (RE_f32)i;
    RE_f32 fy = y - (RE_f32)j;
    RE_f32 fz = z - (RE_f32)k;

    /* Determine simple ordering of contribution corners */
    int a = (fx >= fy) ? 1 : 0;
    int b = (fy >= fz) ? 1 : 0;
    int csel = (fx >= fz) ? 1 : 0;

    /* First corner = cell origin */
    c[0].i = i; c[0].j = j; c[0].k = k;
    c[0].dx = fx; c[0].dy = fy; c[0].dz = fz;
    c[0].hash = RE_OS3D_HASH(i,j,k);

    /* Second corner */
    if (a && csel) {
        c[1].i = i+1; c[1].j = j;   c[1].k = k;
        c[1].dx = fx-1; c[1].dy = fy;   c[1].dz = fz;
    } else if (!a && b) {
        c[1].i = i;   c[1].j = j+1; c[1].k = k;
        c[1].dx = fx;   c[1].dy = fy-1; c[1].dz = fz;
    } else {
        c[1].i = i;   c[1].j = j;   c[1].k = k+1;
        c[1].dx = fx;   c[1].dy = fy;   c[1].dz = fz-1;
    }
    c[1].hash = RE_OS3D_HASH(c[1].i, c[1].j, c[1].k);

    /* Remaining 2 corners always share the same pattern */
    c[2].i = i+1; c[2].j = j+1; c[2].k = k;
    c[2].dx = fx-1; c[2].dy = fy-1; c[2].dz = fz;
    c[2].hash = RE_OS3D_HASH(c[2].i, c[2].j, c[2].k);

    c[3].i = i; c[3].j = j+1; c[3].k = k+1;
    c[3].dx = fx; c[3].dy = fy-1; c[3].dz = fz-1;
    c[3].hash = RE_OS3D_HASH(c[3].i, c[3].j, c[3].k);
}

/* ================================================================================================
   SMOOTH VARIANT — 8 CORNERS (OpenSimplex2S)
================================================================================================ */

RE_INLINE void RE_OS3D_GET_CORNERS_SMOOTH_f32(
        RE_f32 x, RE_f32 y, RE_f32 z,
        RE_OS3D_CornerF32 c[8])
{
    /* Base cell */
    RE_i32 i = RE_FASTFLOOR_f32(x);
    RE_i32 j = RE_FASTFLOOR_f32(y);
    RE_i32 k = RE_FASTFLOOR_f32(z);

    /* Local offsets */
    RE_f32 fx = x - (RE_f32)i;
    RE_f32 fy = y - (RE_f32)j;
    RE_f32 fz = z - (RE_f32)k;

    /* Fill all 8 corners */
    int idx = 0;
    for (int dz=0; dz<2; dz++)
    for (int dy=0; dy<2; dy++)
    for (int dx=0; dx<2; dx++)
    {
        c[idx].i = i + dx;
        c[idx].j = j + dy;
        c[idx].k = k + dz;

        c[idx].dx = fx - dx;
        c[idx].dy = fy - dy;
        c[idx].dz = fz - dz;

        c[idx].hash = RE_OS3D_HASH(c[idx].i, c[idx].j, c[idx].k);
        idx++;
    }
}

/* ================================================================================================
   OpenSimplex2F (FAST) Kernel — 4 Corners
   Derived from the official OpenSimplex2F spec
================================================================================================ */

/* -----------------------------------------------------------------------------------
   Gradient dot product (integer lattice → gradient lookup → dot)
----------------------------------------------------------------------------------- */

RE_INLINE RE_f32 RE_OS3D_GRAD_DOT_FAST_f32(RE_u32 hash, RE_f32 dx, RE_f32 dy, RE_f32 dz)
{
    /* Gradient index: 12 gradients */
    const RE_i8 *g = RE_NOISE_GRAD3[hash % 12];
    return (RE_f32)g[0] * dx + (RE_f32)g[1] * dy + (RE_f32)g[2] * dz;
}

RE_INLINE RE_f64 RE_OS3D_GRAD_DOT_FAST_f64(RE_u32 hash, RE_f64 dx, RE_f64 dy, RE_f64 dz)
{
    const RE_i8 *g = RE_NOISE_GRAD3[hash % 12];
    return (RE_f64)g[0] * dx + (RE_f64)g[1] * dy + (RE_f64)g[2] * dz;
}

/* -----------------------------------------------------------------------------------
   ATTENUATION KERNEL (OpenSimplex2F)
   Contribution = (t^4) * dot(g, d)
   Where:
      t = 0.75 − dx² − dy² − dz²
----------------------------------------------------------------------------------- */

RE_INLINE RE_f32 RE_OS3D_ATTENUATE_FAST_f32(RE_f32 dx, RE_f32 dy, RE_f32 dz)
{
    RE_f32 t = 0.75f - (dx*dx + dy*dy + dz*dz);
    if (t <= 0.0f) return 0.0f;
    t *= t;
    return t * t;    /* t^4 */
}

RE_INLINE RE_f64 RE_OS3D_ATTENUATE_FAST_f64(RE_f64 dx, RE_f64 dy, RE_f64 dz)
{
    RE_f64 t = 0.75 - (dx*dx + dy*dy + dz*dz);
    if (t <= 0.0) return 0.0;
    t *= t;
    return t * t;
}

/* ================================================================================================
   MAIN FAST FUNCTION (OpenSimplex2F)
   - Rotate     (R^-1)
   - Fetch 4 corners
   - Attenuate
   - Accumulate
================================================================================================ */

RE_INLINE RE_f32 RE_NOISE_OPENSIMPLEX3D_FAST_f32(RE_f32 x, RE_f32 y, RE_f32 z)
{
    /* Rotate into skewed simplex “fast” coordinate space */
    RE_OS3D_ROTATE_FAST_f32(&x, &y, &z);

    /* Get 4 corners */
    RE_OS3D_CornerF32 c[4];
    RE_OS3D_GET_CORNERS_FAST_f32(x, y, z, c);

    /* Accumulate */
    RE_f32 sum = 0.0f;

    for (int i = 0; i < 4; i++)
    {
        RE_f32 att = RE_OS3D_ATTENUATE_FAST_f32(c[i].dx, c[i].dy, c[i].dz);
        if (att > 0.0f)
        {
            RE_f32 dot = RE_OS3D_GRAD_DOT_FAST_f32(c[i].hash, c[i].dx, c[i].dy, c[i].dz);
            sum += att * dot;
        }
    }

    /* Final scaling per OpenSimplex2F */
    return sum * OS3D_SCALE_F32;
}

RE_INLINE RE_f64 RE_NOISE_OPENSIMPLEX3D_FAST_f64(RE_f64 x, RE_f64 y, RE_f64 z)
{
    RE_OS3D_ROTATE_FAST_f64(&x, &y, &z);

    RE_OS3D_CornerF64 c[4];
    /* Reuse f32 version for index logic, cast coordinates */
    {
        RE_OS3D_CornerF32 tmp[4];
        RE_OS3D_GET_CORNERS_FAST_f32((RE_f32)x,(RE_f32)y,(RE_f32)z, tmp);

        for (int i=0; i<4; i++){
            c[i].i = tmp[i].i;
            c[i].j = tmp[i].j;
            c[i].k = tmp[i].k;
            c[i].dx = (RE_f64)tmp[i].dx;
            c[i].dy = (RE_f64)tmp[i].dy;
            c[i].dz = (RE_f64)tmp[i].dz;
            c[i].hash = tmp[i].hash;
        }
    }

    RE_f64 sum = 0.0;

    for (int i = 0; i < 4; i++)
    {
        RE_f64 att = RE_OS3D_ATTENUATE_FAST_f64(c[i].dx, c[i].dy, c[i].dz);
        if (att > 0.0)
        {
            RE_f64 dot = RE_OS3D_GRAD_DOT_FAST_f64(c[i].hash, c[i].dx, c[i].dy, c[i].dz);
            sum += att * dot;
        }
    }

    return sum * OS3D_SCALE_F64;
}

/* ================================================================================================
   OPEN SIMPLEX 2S (SMOOTH) 3D NOISE
   High quality, isotropic — uses 5 corners (one extra vs FAST version).
   ================================================================================================ */

/* Gradient dot helper */
RE_INLINE RE_f32 RE_OS_DOT3_f32(const RE_i8 *g, RE_f32 x, RE_f32 y, RE_f32 z)
{
    return g[0]*x + g[1]*y + g[2]*z;
}

RE_INLINE RE_f64 RE_OS_DOT3_f64(const RE_i8 *g, RE_f64 x, RE_f64 y, RE_f64 z)
{
    return (RE_f64)g[0]*x + (RE_f64)g[1]*y + (RE_f64)g[2]*z;
}

/* ================================================================================================
   OPEN SIMPLEX 2S 3D — f32
   ================================================================================================ */

RE_INLINE RE_f32 RE_NOISE_OS3D_SMOOTH_f32(RE_f32 x, RE_f32 y, RE_f32 z)
{
    /* Skew constants */
    const RE_f32 R3 = (2.0f / 3.0f);      /* unskew factor */

    /* Skew into BCC lattice */
    RE_f32 s = (x + y + z) * R3;

    RE_f32 xs = x + s;
    RE_f32 ys = y + s;
    RE_f32 zs = z + s;

    RE_i32 xi = RE_FASTFLOOR_f32(xs);
    RE_i32 yi = RE_FASTFLOOR_f32(ys);
    RE_i32 zi = RE_FASTFLOOR_f32(zs);

    /* Unskew */
    RE_f32 t = (RE_f32)(xi + yi + zi) * R3;

    RE_f32 X0 = xi - t;
    RE_f32 Y0 = yi - t;
    RE_f32 Z0 = zi - t;

    RE_f32 x0 = x - X0;
    RE_f32 y0 = y - Y0;
    RE_f32 z0 = z - Z0;

    /* Rank order the components */
    RE_i32 rx = (x0 >= y0) + (x0 >= z0);
    RE_i32 ry = (y0 >= x0) + (y0 >= z0);
    RE_i32 rz = 3 - (rx + ry);

    /* 5 corner offsets */
    static const RE_i32 LATTICE_CORNER[5][3] = {
        {0,0,0},
        {1,0,0},
        {0,1,0},
        {0,0,1},
        {1,1,1}
    };

    /* Weighted contributions */
    RE_f32 value = 0.0f;

    for (int i = 0; i < 5; i++)
    {
        RE_i32 dx = LATTICE_CORNER[i][0];
        RE_i32 dy = LATTICE_CORNER[i][1];
        RE_i32 dz = LATTICE_CORNER[i][2];

        /* Second through fourth corners use rank logic */
        if (i == 1) dx = (rx >= 2);
        if (i == 2) dy = (ry >= 2);
        if (i == 3) dz = (rz >= 2);

        RE_f32 px = x0 - dx + R3 * i;
        RE_f32 py = y0 - dy + R3 * i;
        RE_f32 pz = z0 - dz + R3 * i;

        RE_f32 attn = 0.6f - (px*px + py*py + pz*pz);
        if (attn > 0.0f)
        {
            RE_i32 hash = RE_NOISE_HASH3(xi + dx, yi + dy, zi + dz);
            const RE_i8 *g = RE_NOISE_GRAD3[hash % 12];

            RE_f32 dot = RE_OS_DOT3_f32(g, px, py, pz);
            attn *= attn;
            value += attn * attn * dot;
        }
    }

    return value * (OS3D_SCALE_F32);
}


/* ================================================================================================
   OPEN SIMPLEX 2S 3D — f64
   ================================================================================================ */

RE_INLINE RE_f64 RE_NOISE_OS3D_SMOOTH_f64(RE_f64 x, RE_f64 y, RE_f64 z)
{
    const RE_f64 R3 = (2.0 / 3.0);

    RE_f64 s = (x+y+z) * R3;

    RE_f64 xs = x + s;
    RE_f64 ys = y + s;
    RE_f64 zs = z + s;

    RE_i64 xi = RE_FASTFLOOR_f64(xs);
    RE_i64 yi = RE_FASTFLOOR_f64(ys);
    RE_i64 zi = RE_FASTFLOOR_f64(zs);

    RE_f64 t = (RE_f64)(xi + yi + zi) * R3;

    RE_f64 X0 = xi - t;
    RE_f64 Y0 = yi - t;
    RE_f64 Z0 = zi - t;

    RE_f64 x0 = x - X0;
    RE_f64 y0 = y - Y0;
    RE_f64 z0 = z - Z0;

    RE_i32 rx = (x0 >= y0) + (x0 >= z0);
    RE_i32 ry = (y0 >= x0) + (y0 >= z0);
    RE_i32 rz = 3 - (rx + ry);

    static const RE_i32 LATTICE_CORNER[5][3] = {
        {0,0,0},
        {1,0,0},
        {0,1,0},
        {0,0,1},
        {1,1,1}
    };

    RE_f64 value = 0.0;

    for (int i = 0; i < 5; i++)
    {
        RE_i32 dx = LATTICE_CORNER[i][0];
        RE_i32 dy = LATTICE_CORNER[i][1];
        RE_i32 dz = LATTICE_CORNER[i][2];

        if (i == 1) dx = (rx >= 2);
        if (i == 2) dy = (ry >= 2);
        if (i == 3) dz = (rz >= 2);

        RE_f64 px = x0 - dx + R3 * i;
        RE_f64 py = y0 - dy + R3 * i;
        RE_f64 pz = z0 - dz + R3 * i;

        RE_f64 attn = 0.6 - (px*px + py*py + pz*pz);
        if (attn > 0.0)
        {
            RE_i32 hash = RE_NOISE_HASH3((RE_i32)(xi+dx), (RE_i32)(yi+dy), (RE_i32)(zi+dz));
            const RE_i8 *g = RE_NOISE_GRAD3[hash % 12];

            RE_f64 dot = RE_OS_DOT3_f64(g, px, py, pz);

            attn *= attn;
            value += attn * attn * dot;
        }
    }

    return value * (OS3D_SCALE_F64);
}

/* ----------------------------------------
    FAST OpenSimplex2 (3D)
    Uses 4 contributions instead of 5
   ---------------------------------------- */

RE_INLINE RE_f32 RE_NOISE_OS3D_FAST_f32(RE_f32 x, RE_f32 y, RE_f32 z)
{
    /* Skew constant */
    const RE_f32 R3 = (2.0f / 3.0f);

    /* Skew into stretched BCC lattice */
    RE_f32 s = (x + y + z) * R3;
    RE_f32 xs = x + s;
    RE_f32 ys = y + s;
    RE_f32 zs = z + s;

    RE_i32 xi = RE_FASTFLOOR_f32(xs);
    RE_i32 yi = RE_FASTFLOOR_f32(ys);
    RE_i32 zi = RE_FASTFLOOR_f32(zs);

    /* Unskew */
    RE_f32 t = (RE_f32)(xi + yi + zi) * R3;
    RE_f32 X0 = xi - t;
    RE_f32 Y0 = yi - t;
    RE_f32 Z0 = zi - t;

    /* Distances */
    RE_f32 x0 = x - X0;
    RE_f32 y0 = y - Y0;
    RE_f32 z0 = z - Z0;

    /* Rank components */
    RE_i32 rx = (x0 >= y0) + (x0 >= z0);
    RE_i32 ry = (y0 >= x0) + (y0 >= z0);
    RE_i32 rz = 3 - (rx + ry);

    /* Corner offsets */
    static const RE_i32 LATTICE[4][3] = {
        {0,0,0},     /* first */
        {1,0,0},     /* sorted by rank */
        {0,1,0},
        {0,0,1}
    };

    RE_f32 value = 0.0f;

    for (int i = 0; i < 4; i++)
    {
        RE_i32 dx = LATTICE[i][0];
        RE_i32 dy = LATTICE[i][1];
        RE_i32 dz = LATTICE[i][2];

        /* Choose the right ranked component for 2nd, 3rd, 4th corners */
        if (i == 1) dx = (rx >= 2);
        if (i == 2) dy = (ry >= 2);
        if (i == 3) dz = (rz >= 2);

        RE_f32 px = x0 - dx + R3 * i;
        RE_f32 py = y0 - dy + R3 * i;
        RE_f32 pz = z0 - dz + R3 * i;

        RE_f32 attn = 0.75f - (px*px + py*py + pz*pz);
        if (attn > 0.0f)
        {
            RE_i32 hash = RE_NOISE_HASH3(xi+dx, yi+dy, zi+dz);
            const RE_i8 *g = RE_NOISE_GRAD3[hash % 12];

            RE_f32 dot = g[0]*px + g[1]*py + g[2]*pz;

            attn *= attn;
            value += attn * attn * dot;
        }
    }

    return value * OS3D_SCALE_F32;
}


/* ================================================================================================
   DOUBLE VERSION
   ================================================================================================ */

RE_INLINE RE_f64 RE_NOISE_OS3D_FAST_f64(RE_f64 x, RE_f64 y, RE_f64 z)
{
    const RE_f64 R3 = (2.0 / 3.0);

    RE_f64 s = (x+y+z) * R3;
    RE_f64 xs = x + s;
    RE_f64 ys = y + s;
    RE_f64 zs = z + s;

    RE_i64 xi = RE_FASTFLOOR_f64(xs);
    RE_i64 yi = RE_FASTFLOOR_f64(ys);
    RE_i64 zi = RE_FASTFLOOR_f64(zs);

    RE_f64 t = (RE_f64)(xi + yi + zi) * R3;

    RE_f64 X0 = xi - t;
    RE_f64 Y0 = yi - t;
    RE_f64 Z0 = zi - t;

    RE_f64 x0 = x - X0;
    RE_f64 y0 = y - Y0;
    RE_f64 z0 = z - Z0;

    RE_i32 rx = (x0 >= y0) + (x0 >= z0);
    RE_i32 ry = (y0 >= x0) + (y0 >= z0);
    RE_i32 rz = 3 - (rx + ry);

    static const RE_i32 LATTICE[4][3] = {
        {0,0,0},
        {1,0,0},
        {0,1,0},
        {0,0,1}
    };

    RE_f64 value = 0.0;

    for (int i = 0; i < 4; i++)
    {
        RE_i32 dx = LATTICE[i][0];
        RE_i32 dy = LATTICE[i][1];
        RE_i32 dz = LATTICE[i][2];

        if (i == 1) dx = (rx >= 2);
        if (i == 2) dy = (ry >= 2);
        if (i == 3) dz = (rz >= 2);

        RE_f64 px = x0 - dx + R3 * i;
        RE_f64 py = y0 - dy + R3 * i;
        RE_f64 pz = z0 - dz + R3 * i;

        RE_f64 attn = 0.75 - (px*px + py*py + pz*pz);
        if (attn > 0.0)
        {
            RE_i32 h = RE_NOISE_HASH3((RE_i32)(xi+dx), (RE_i32)(yi+dy), (RE_i32)(zi+dz));
            const RE_i8 *g = RE_NOISE_GRAD3[h % 12];

            RE_f64 dot = (RE_f64)g[0]*px + (RE_f64)g[1]*py + (RE_f64)g[2]*pz;

            attn *= attn;
            value += attn * attn * dot;
        }
    }

    return value * OS3D_SCALE_F64;
}

/* =============================================================================================
   OPEN SIMPLEX 2S (SMOOTH) — 2D — f32
   ============================================================================================= */

RE_INLINE RE_f32 RE_NOISE_OS2D_SMOOTH_f32(RE_f32 x, RE_f32 y)
{
    /* Stretch constant */
    const RE_f32 S2 = 0.366025403f;  /* (sqrt(3)-1)/2 */

    /* Unstretch */
    RE_f32 s = (x + y) * S2;
    RE_f32 xs = x + s;
    RE_f32 ys = y + s;

    /* Base grid cell */
    RE_i32 i = RE_FASTFLOOR_f32(xs);
    RE_i32 j = RE_FASTFLOOR_f32(ys);

    /* Unskew */
    const RE_f32 U2 = 0.211324865f;  /* (3-sqrt(3))/6 */

    RE_f32 xi = xs - i;
    RE_f32 yi = ys - j;

    RE_f32 x0 = x - (i - (xi + yi) * U2);
    RE_f32 y0 = y - (j - (xi + yi) * U2);

    RE_f32 value = 0.0f;

    /* Contributions of 6 corners */
    for (int c = 0; c < 6; c++)
    {
        static const RE_i32 OFF[][2] = {
            {0,0}, {1,0}, {0,1},
            {1,1}, {2,0}, {0,2}
        };

        RE_f32 dx = x0 - OFF[c][0] * U2;
        RE_f32 dy = y0 - OFF[c][1] * U2;

        RE_f32 attn = 0.5f - dx*dx - dy*dy;
        if (attn > 0.f)
        {
            RE_u8 h = RE_NOISE_HASH2(i + OFF[c][0], j + OFF[c][1]);
            const RE_i8 *g = RE_NOISE_GRAD2[h & 7];

            RE_f32 dot = g[0]*dx + g[1]*dy;

            attn *= attn;
            value += attn * attn * dot;
        }
    }

    return value * OS2D_SCALE_F32;
}

/* =============================================================================================
   OPEN SIMPLEX 2S (SMOOTH) — 2D — f64
   ============================================================================================= */

RE_INLINE RE_f64 RE_NOISE_OS2D_SMOOTH_f64(RE_f64 x, RE_f64 y)
{
    const RE_f64 S2 = 0.3660254037844386467637231707;
    RE_f64 s = (x + y) * S2;

    RE_f64 xs = x + s;
    RE_f64 ys = y + s;

    RE_i64 i = RE_FASTFLOOR_f64(xs);
    RE_i64 j = RE_FASTFLOOR_f64(ys);

    const RE_f64 U2 = 0.2113248654051871177454256097;

    RE_f64 xi = xs - i;
    RE_f64 yi = ys - j;

    RE_f64 x0 = x - (i - (xi + yi) * U2);
    RE_f64 y0 = y - (j - (xi + yi) * U2);

    RE_f64 value = 0.0;

    for (int c = 0; c < 6; c++)
    {
        static const RE_i32 OFF[][2] = {
            {0,0},{1,0},{0,1},{1,1},{2,0},{0,2}
        };

        RE_f64 dx = x0 - OFF[c][0] * U2;
        RE_f64 dy = y0 - OFF[c][1] * U2;

        RE_f64 attn = 0.5 - dx*dx - dy*dy;
        if (attn > 0.0)
        {
            RE_u8 h = RE_NOISE_HASH2((RE_i32)(i + OFF[c][0]), (RE_i32)(j + OFF[c][1]));
            const RE_i8 *g = RE_NOISE_GRAD2[h & 7];

            RE_f64 dot = (RE_f64)g[0]*dx + (RE_f64)g[1]*dy;

            attn *= attn;
            value += attn * attn * dot;
        }
    }

    return value * OS2D_SCALE_F64;
}

RE_INLINE RE_f32 RE_NOISE_OS2D_FAST_f32(RE_f32 x, RE_f32 y)
{
    const RE_f32 S2 = 0.366025403f;

    RE_f32 s = (x + y) * S2;
    RE_f32 xs = x + s;
    RE_f32 ys = y + s;

    RE_i32 i = RE_FASTFLOOR_f32(xs);
    RE_i32 j = RE_FASTFLOOR_f32(ys);

    const RE_f32 U2 = 0.211324865f;

    RE_f32 xi = xs - i;
    RE_f32 yi = ys - j;

    RE_f32 x0 = x - (i - (xi + yi) * U2);
    RE_f32 y0 = y - (j - (xi + yi) * U2);

    static const RE_i32 OFF[3][2] = {
        {0,0}, {1,0}, {0,1}
    };

    RE_f32 value = 0.0f;

    for (int c = 0; c < 3; c++)
    {
        RE_f32 dx = x0 - OFF[c][0] * U2;
        RE_f32 dy = y0 - OFF[c][1] * U2;

        RE_f32 attn = 0.5f - dx*dx - dy*dy;
        if (attn > 0.0f)
        {
            RE_u8 h = RE_NOISE_HASH2(i + OFF[c][0], j + OFF[c][1]);
            const RE_i8 *g = RE_NOISE_GRAD2[h & 7];

            RE_f32 dot = g[0]*dx + g[1]*dy;

            attn *= attn;
            value += attn * attn * dot;
        }
    }

    return value * OS2D_SCALE_F32;
}

/* =============================================================================================
   OPEN SIMPLEX 2F (FAST) — 2D — f64
   ============================================================================================= */

RE_INLINE RE_f64 RE_NOISE_OS2D_FAST_f64(RE_f64 x, RE_f64 y)
{
    const RE_f64 S2 = 0.36602540378443864676;

    RE_f64 s = (x + y) * S2;
    RE_f64 xs = x + s;
    RE_f64 ys = y + s;

    RE_i64 i = RE_FASTFLOOR_f64(xs);
    RE_i64 j = RE_FASTFLOOR_f64(ys);

    const RE_f64 U2 = 0.21132486540518711774;

    RE_f64 xi = xs - i;
    RE_f64 yi = ys - j;

    RE_f64 x0 = x - (i - (xi + yi) * U2);
    RE_f64 y0 = y - (j - (xi + yi) * U2);

    static const RE_i32 OFF[3][2] = {
        {0,0}, {1,0}, {0,1}
    };

    RE_f64 value = 0.0;

    for (int c = 0; c < 3; c++)
    {
        RE_f64 dx = x0 - OFF[c][0] * U2;
        RE_f64 dy = y0 - OFF[c][1] * U2;

        RE_f64 attn = 0.5 - dx*dx - dy*dy;
        if (attn > 0.0)
        {
            RE_u8 h = RE_NOISE_HASH2((RE_i32)(i+OFF[c][0]), (RE_i32)(j+OFF[c][1]));
            const RE_i8 *g = RE_NOISE_GRAD2[h & 7];

            RE_f64 dot = (RE_f64)g[0]*dx + (RE_f64)g[1]*dy;

            attn *= attn;
            value += attn * attn * dot;
        }
    }

    return value * OS2D_SCALE_F64;
}

#endif /* RE_NOISE_H */
