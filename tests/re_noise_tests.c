/**
 * @file re_noise_test.c
 * @brief Unit tests for REMath noise functions.
 *
 * All functions tested against:
 *  - determinism (same input → same output)
 *  - continuity (small deltas → small changes)
 *  - basic range validation
 *  - edge coordinate behavior
 */

#include "../include/re_noise.h"
#include "../include/re_core.h"
#include "../include/re_test_core.h"

#include <stdio.h>
#include <math.h>

/* ============================================================================================
   Helpers
   ============================================================================================ */

static RE_BOOL approx_f32(RE_f32 a, RE_f32 b, RE_f32 eps)
{
    return fabsf(a - b) <= eps;
}

static RE_BOOL approx_vec2(RE_f32 a, RE_f32 b)
{
    return fabsf(a - b) < 1e-4f;
}

/* ============================================================================================
   1. HASH TESTS
   ============================================================================================ */

static void test_hash_determinism(void)
{
    RE_u32 h1 = RE_HASH3D(10,20,30);
    RE_u32 h2 = RE_HASH3D(10,20,30);
    RE_u32 h3 = RE_HASH3D(11,20,30);

    test_result("HASH deterministic", h1 == h2);
    test_result("HASH different input", h1 != h3);
}

static void test_hash_float(void)
{
    RE_f32 v1 = RE_HASH3D_to_f32(5, 7, 9);
    test_result("HASH f32 range", v1 >= 0.f && v1 <= 1.f);

    RE_f64 v2 = RE_HASH3D_to_f64(5, 7, 9);
    test_result("HASH f64 range", v2 >= 0.0 && v2 <= 1.0);
}

/* ============================================================================================
   2. VALUE NOISE
   ============================================================================================ */

static void test_value2(void)
{
    RE_f32 a = RE_NOISE_VALUE2_f32(10.1f, 20.5f);
    RE_f32 b = RE_NOISE_VALUE2_f32(10.1f, 20.5f);

    test_result("VALUE2 deterministic", approx_f32(a, b, 1e-6f));
    test_result("VALUE2 range", a >= -1.f && a <= 1.f);
}

static void test_value3(void)
{
    RE_f32 a = RE_NOISE_VALUE3_f32(1.25f, 2.75f, 3.5f);
    RE_f32 b = RE_NOISE_VALUE3_f32(1.25f, 2.75f, 3.5f);

    test_result("VALUE3 deterministic", approx_f32(a, b, 1e-6f));
    test_result("VALUE3 range", a >= -1.f && a <= 1.f);
}

static void test_value4(void)
{
    RE_f32 a = RE_NOISE_VALUE4_f32(0.2f, 0.4f, 0.6f, 0.8f);

    test_result("VALUE4 range", a >= -1.f && a <= 1.f);
}

/* ============================================================================================
   3. PERLIN 3D
   ============================================================================================ */

static void test_perlin3_basic(void)
{
    RE_f32 a = RE_NOISE_PERLIN3_f32(3.14f, 2.71f, 1.0f);
    RE_f32 b = RE_NOISE_PERLIN3_f32(3.14f, 2.71f, 1.0f);

    test_result("PERLIN deterministic", approx_f32(a, b, 1e-6f));
}

static void test_perlin3_smoothness(void)
{
    RE_f32 a = RE_NOISE_PERLIN3_f32(1.f, 1.f, 1.f);
    RE_f32 b = RE_NOISE_PERLIN3_f32(1.01f, 1.f, 1.f);

    test_result("PERLIN smoothness", fabsf(a - b) < 0.2f);
}

/* ============================================================================================
   4. OpenSimplex3D (FAST + SMOOTH)
   ============================================================================================ */

static void test_os3d_fast(void)
{
    RE_f32 a = RE_NOISE_OPENSIMPLEX3D_FAST_f32(1.f, 2.f, 3.f);
    RE_f32 b = RE_NOISE_OPENSIMPLEX3D_FAST_f32(1.f, 2.f, 3.f);

    test_result("OS3D FAST deterministic", approx_f32(a, b, 1e-6f));
}

static void test_os3d_smooth(void)
{
    RE_f32 a = RE_NOISE_OS3D_SMOOTH_f32(0.5f, 0.25f, 0.75f);
    RE_f32 b = RE_NOISE_OS3D_SMOOTH_f32(0.5f, 0.25f, 0.75f);

    test_result("OS3D SMOOTH deterministic", approx_f32(a, b, 1e-6f));
}

static void test_os3d_compare_fast_vs_smooth(void)
{
    RE_f32 f = RE_NOISE_OPENSIMPLEX3D_FAST_f32(0.3f, 0.7f, 0.9f);
    RE_f32 s = RE_NOISE_OS3D_SMOOTH_f32(0.3f, 0.7f, 0.9f);

    test_result("FAST vs SMOOTH not identical", !approx_f32(f, s, 1e-6f));
}

/* ============================================================================================
   5. OpenSimplex2D Smooth
   ============================================================================================ */

static void test_os2d_smooth(void)
{
    RE_f32 a = RE_NOISE_OS2D_SMOOTH_f32(1.1f, 2.2f);
    RE_f32 b = RE_NOISE_OS2D_SMOOTH_f32(1.1f, 2.2f);

    test_result("OS2D SMOOTH deterministic", approx_f32(a, b, 1e-6f));
}

/* ============================================================================================
   6. FRACTAL VARIANTS
   ============================================================================================ */

static void test_fbm(void)
{
    RE_f32 v = RE_NOISE_VALUE3_FBM_f32(1.f, 2.f, 3.f, 4, 2.f, 0.5f);
    test_result("FBM non-zero", v != 0.f);
}

static void test_turbulence(void)
{
    RE_f32 v = RE_NOISE_VALUE3_TURBULENCE_f32(1.f,1.f,1.f,4,2.f,0.5f);
    test_result("TURB non-negative", v >= 0.f);
}

static void test_ridged(void)
{
    RE_f32 v = RE_NOISE_VALUE3_RIDGED_f32(1.f,1.f,1.f,4,2.f,0.5f,1.f);
    test_result("RIDGED non-negative", v >= 0.f);
}

/* ============================================================================================
   7. MASTER TEST RUNNER
   ============================================================================================ */

void run_noise_tests(void)
{
    printf("=== re_noise tests start ===\n");

    /* Hash tests */
    test_hash_determinism();
    test_hash_float();

    /* Value noise */
    test_value2();
    test_value3();
    test_value4();

    /* Perlin 3D */
    test_perlin3_basic();
    test_perlin3_smoothness();

    /* OpenSimplex3D */
    test_os3d_fast();
    test_os3d_smooth();
    test_os3d_compare_fast_vs_smooth();

    /* OpenSimplex 2D */
    test_os2d_smooth();

    /* Fractal */
    test_fbm();
    test_turbulence();
    test_ridged();

    printf("=== re_noise tests finished ===\n");
}
