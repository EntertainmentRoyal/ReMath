/**
 * @file re_random_test.c
 * @brief Test suite for REMath random system (PCG32-based).
 */

#include <math.h>
#include <stdio.h>
#include "../include/re_random.h"
#include "../include/re_test_core.h"
#include "../include/re_vec.h"
#include "../include/re_quat.h"

/* ============================================================================================
   Helpers
   ============================================================================================ */

static RE_BOOL approx_f32(RE_f32 a, RE_f32 b, RE_f32 eps)
{
    return fabsf(a - b) <= eps;
}

static RE_BOOL approx_len1_v3(RE_V3_f32 v)
{
    RE_f32 L = RE_SQRT(v.x*v.x + v.y*v.y + v.z*v.z);
    return approx_f32(L, 1.0f, 1e-3f);
}

static RE_BOOL approx_len1_quat(RE_QUAT_f32 q)
{
    RE_f32 L = RE_SQRT(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
    return approx_f32(L, 1.0f, 1e-3f);
}

/* ============================================================================================
   TESTS
   ============================================================================================ */

static void test_seed_determinism(void)
{
    RE_RANDOM_STATE A = RE_RANDOM_SEED(1234, 5678);
    RE_RANDOM_STATE B = RE_RANDOM_SEED(1234, 5678);

    RE_BOOL same = RE_TRUE;
    for (int i = 0; i < 8; i++)
        if (RE_RANDOM_U32(&A) != RE_RANDOM_U32(&B))
            same = RE_FALSE;

    test_result("RANDOM determinism with same seed", same);
}

static void test_seed_variation(void)
{
    RE_RANDOM_STATE A = RE_RANDOM_SEED(1, 1);
    RE_RANDOM_STATE B = RE_RANDOM_SEED(2, 2);

    RE_u32 a = RE_RANDOM_U32(&A);
    RE_u32 b = RE_RANDOM_U32(&B);

    test_result("RANDOM different seed -> different values", a != b);
}

static void test_f32_range(void)
{
    RE_RANDOM_STATE rng = RE_RANDOM_SEED(1337, 1);

    RE_BOOL ok = RE_TRUE;
    for (int i = 0; i < 1000; i++)
    {
        RE_f32 v = RE_RANDOM_F32(&rng);
        if (v < 0.0f || v >= 1.0f)
            ok = RE_FALSE;
    }

    test_result("RANDOM_F32 in [0,1)", ok);
}

static void test_range_functions(void)
{
    RE_RANDOM_STATE rng = RE_RANDOM_SEED(999, 123);

    RE_BOOL ok_u32 = RE_TRUE;
    RE_BOOL ok_f32 = RE_TRUE;

    for (int i = 0; i < 500; i++)
    {
        RE_u32 u = RE_RANDOM_RANGE_U32(&rng, 10, 20);
        if (u < 10 || u > 20) ok_u32 = RE_FALSE;

        RE_f32 f = RE_RANDOM_RANGE_F32(&rng, -5.0f, 5.0f);
        if (f < -5.0f || f > 5.0f) ok_f32 = RE_FALSE;
    }

    test_result("RANGE_U32 [10..20]", ok_u32);
    test_result("RANGE_F32 [-5..5]", ok_f32);
}

static void test_float_distribution(void)
{
    RE_RANDOM_STATE rng = RE_RANDOM_SEED(12345, 6789);

    /* Check mean roughly ~0.5 */
    RE_f32 sum = 0.f;
    const int samples = 20000;

    for (int i = 0; i < samples; i++)
        sum += RE_RANDOM_F32(&rng);

    RE_f32 mean = sum / samples;

    test_result("RANDOM_F32 mean ~ 0.5", fabsf(mean - 0.5f) < 0.05f);
}

static void test_unit_vectors(void)
{
    RE_RANDOM_STATE rng = RE_RANDOM_SEED(777, 999);

    RE_V2_f32 v2 = RE_RANDOM_UNIT2_F32(&rng);
    RE_f32 len2 = RE_SQRT(v2.x*v2.x + v2.y*v2.y);

    test_result("UNIT2 length approx 1", approx_f32(len2, 1.0f, 1e-3f));

    RE_V3_f32 v3 = RE_RANDOM_UNIT3_F32(&rng);
    test_result("UNIT3 length approx 1", approx_len1_v3(v3));
}

static void test_random_quat(void)
{
    RE_RANDOM_STATE rng = RE_RANDOM_SEED(42, 777);

    RE_QUAT_f32 q = RE_RANDOM_QUAT_F32(&rng);

    test_result("RANDOM QUAT length approx 1", approx_len1_quat(q));

    /* ensure not zero quaternion */
    test_result("RANDOM QUAT non-zero",
        !(q.x==0 && q.y==0 && q.z==0 && q.w==0));
}

/* ============================================================================================
   Entry Point
   ============================================================================================ */

void run_random_tests(void)
{
    printf("=== Random tests start ===\n");

    test_seed_determinism();
    test_seed_variation();
    test_f32_range();
    test_range_functions();
    test_float_distribution();
    test_unit_vectors();
    test_random_quat();

    printf("=== Random tests end ===\n");
}
