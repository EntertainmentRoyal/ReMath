/**
 * @file re_math_test.c
 * @brief Unit tests for REMath core mathematical helper functions.
 *
 * This suite verifies correctness, edge–case behavior, IEEE-754
 * consistency, and REMath-specific behavior of all inline math
 * utilities defined in `re_math.h`.
 *
 * @author
 * Jayansh Devgan
 */

#include "../include/re_math.h"
#include "../include/re_core.h"
#include "../include/re_test_core.h"

#include <stdio.h>
#include <math.h>
#include <assert.h>

/* ============================================================================================
   TEST HELPERS
   ============================================================================================ */

/**
 * @brief Approximate float comparison helper.
 * @param a First value.
 * @param b Second value.
 * @param eps Allowed error tolerance.
 * @return RE_true if |a - b| <= eps.
 */
static RE_BOOL approx_eq_f32(RE_f32 a, RE_f32 b, RE_f32 eps)
{
    return RE_ABS(a - b) <= eps;
}

/* ============================================================================================
   TEST SUITE
   ============================================================================================ */

/**
 * @brief Tests basic ABS, MIN, MAX behavior.
 */
static void test_basic_math(void)
{
    test_result("ABS positive",  RE_ABS(5.0f) == 5.0f);
    test_result("ABS negative",  RE_ABS(-5.0f) == 5.0f);

    test_result("MIN",           RE_MIN(3.0f, 10.f) == 3.0f);
    test_result("MAX",           RE_MAX(3.0f, 10.f) == 10.0f);
}

/**
 * @brief Tests clamp, saturate, lerp.
 */
static void test_clamp_lerp(void)
{
    test_result("CLAMP inside",  RE_CLAMP(5.f, 0.f, 10.f) == 5.f);
    test_result("CLAMP low",     RE_CLAMP(-1.f, 0.f, 10.f) == 0.f);
    test_result("CLAMP high",    RE_CLAMP(50.f, 0.f, 10.f) == 10.f);

    test_result("SATURATE mid",  RE_SATURATE(0.5f) == 0.5f);
    test_result("SATURATE low",  RE_SATURATE(-3.f) == 0.f);
    test_result("SATURATE high", RE_SATURATE(4.f)  == 1.f);

    test_result("LERP mid", approx_eq_f32(RE_LERP(0.f, 10.f, 0.5f), 5.f, 1e-6f));
}

/**
 * @brief Tests Smoothstep interpolation.
 */
static void test_smoothstep(void)
{
    RE_f32 r1 = RE_SMOOTHSTEP(0.f, 1.f, 0.f);
    RE_f32 r2 = RE_SMOOTHSTEP(0.f, 1.f, 1.f);
    RE_f32 r3 = RE_SMOOTHSTEP(0.f, 1.f, 0.5f);

    test_result("SMOOTHSTEP 0", approx_eq_f32(r1, 0.f, 1e-3f));
    test_result("SMOOTHSTEP 1", approx_eq_f32(r2, 1.f, 1e-3f));
    test_result("SMOOTHSTEP mid", r3 > 0.3f && r3 < 0.7f);
}

/**
 * @brief Tests FLOOR, CEIL, ROUND, FRAC including negative numbers.
 */
static void test_rounding(void)
{
    // FLOOR
    test_result("FLOOR 3.7",  approx_eq_f32(RE_FLOOR(3.7f), floorf(3.7f), 1e-5f));
    test_result("FLOOR -3.7", approx_eq_f32(RE_FLOOR(-3.7f), floorf(-3.7f), 1e-5f));

    // // CEIL
    test_result("CEIL 3.2",   approx_eq_f32(RE_CEIL(3.2f), ceilf(3.2f), 1e-5f));
    test_result("CEIL -3.2",  approx_eq_f32(RE_CEIL(-3.2f), ceilf(-3.2f), 1e-5f));

    // ROUND
    test_result("ROUND 3.5",  approx_eq_f32(RE_ROUND(3.5f), roundf(3.5f), 1e-5f));
    test_result("ROUND -3.5", approx_eq_f32(RE_ROUND(-3.5f), roundf(-3.5f), 1e-5f));

    // FRAC
    test_result("FRAC 3.75",  approx_eq_f32(RE_FRAC(3.75f), 0.75f, 1e-6f));
    test_result("FRAC -3.75", approx_eq_f32(RE_FRAC(-3.75f), 1.f - 0.75f, 1e-6f));  // matches x - floor(x)
}

/**
 * @brief Tests SIGN function.
 */
static void test_sign(void)
{
    test_result("SIGN pos", RE_SIGN(5.f) == 1.f);
    test_result("SIGN neg", RE_SIGN(-5.f) == -1.f);
    test_result("SIGN zero", RE_SIGN(0.f) == 0.f);
}

/**
 * @brief Tests power-of-two helpers.
 */
static void test_pow2(void)
{
    test_result("IS_POW2 true",   RE_IS_POW2(8));
    test_result("IS_POW2 false", !RE_IS_POW2(10));

    test_result("NEXT_POW2",
        RE_NEXT_POW2(300) == 512
    );
}

/**
 * @brief Tests NaN, Inf, Finite classification.
 */
static void test_float_classification(void)
{
    RE_f32 nanv = NAN;
    RE_f32 infv = INFINITY;
    RE_f32 ninf = -INFINITY;

    test_result("ISNAN",    RE_ISNAN(nanv));
    test_result("ISINF pos",RE_ISINF(infv));
    test_result("ISINF neg",RE_ISINF(ninf));
    test_result("ISFINITE", RE_ISFINITE(3.14f));
}

/**
 * @brief Runs all REMath tests.
 */
void run_math_tests(void)
{
    printf("=== re_math tests start ===\n");

    test_basic_math();
    test_clamp_lerp();
    test_smoothstep();
    test_rounding();
    test_sign();
    test_pow2();
    test_float_classification();

    printf("=== re_math tests finished ===\n");
}
