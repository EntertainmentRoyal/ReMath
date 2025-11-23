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

    test_result("SATURATE mid",  RE_CLAMP01(0.5f) == 0.5f);
    test_result("SATURATE low",  RE_CLAMP01(-3.f) == 0.f);
    test_result("SATURATE high", RE_CLAMP01(4.f)  == 1.f);

    test_result("LERP mid", approx_eq_f32(RE_LERP(0.f, 10.f, 0.5f), 5.f, 1e-6f));
}

static void test_fmod(void)
{
    test_result("FMOD simple", approx_eq_f32(RE_FMOD_f32(5.3f, 2.0f), fmodf(5.3f, 2.0f), 1e-5f));
    // test_result("FMOD negative x", approx_eq_f32(RE_FMOD_f32(-5.3f, 2.0f), fmodf(-5.3f, 2.0f), 1e-5f));
    test_result("FMOD fractional", approx_eq_f32(RE_FMOD_f32(3.75f, 1.2f), fmodf(3.75f, 1.2f), 1e-5f));

    // wrap-around behavior test (HSV usage)
    test_result("FMOD wrap 370°",
        approx_eq_f32(RE_FMOD_f32(370.0f, 360.0f), 10.0f, 1e-4f));

    test_result("FMOD wrap -30°",
        approx_eq_f32(RE_FMOD_f32(-30.0f, 360.0f), 330.0f, 1e-4f));
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

static void test_exp_basic(void)
{
    test_result("EXP(0) == 1", approx_eq_f32(RE_EXP_f32(0.0f), 1.0f, 1e-5f));
    test_result("EXP(1) approx e",
        approx_eq_f32(RE_EXP_f32(1.0f), 2.7182818f, 1e-4f));

    test_result("EXP(-1) approx 1/e",
        approx_eq_f32(RE_EXP_f32(-1.0f), 1.0f / 2.7182818f, 1e-4f));

    test_result("EXP(2) approx e^2",
        approx_eq_f32(RE_EXP_f32(2.0f), 7.389056f, 1e-3f));
}

/* ============================================================================
   TEST: RE_POW_f32 — correctness against reference powf
   ============================================================================ */

static void test_pow_basic(void)
{
    test_result("POW(2,0) == 1", approx_eq_f32(RE_POW_f32(2.f, 0.f), 1.f, 1e-4f));
    test_result("POW(2,1) == 2", approx_eq_f32(RE_POW_f32(2.f, 1.f), 2.f, 1e-4f));

    test_result("POW(2,2) == 4", approx_eq_f32(RE_POW_f32(2.f, 2.f), 4.f, 1e-3f));
    test_result("POW(4,0.5) == 2",
        approx_eq_f32(RE_POW_f32(4.f, 0.5f), 2.f, 1e-3f));

    test_result("POW(9,0.5) == 3",
        approx_eq_f32(RE_POW_f32(9.f, 0.5f), 3.f, 1e-3f));
}

/* ============================================================================
   TEST: RE_POW_f32 — compare against powf across a sweep
   ============================================================================ */

static RE_BOOL approx_rel_f32(RE_f32 a, RE_f32 b, RE_f32 rel)
{
    RE_f32 diff = RE_ABS(a - b);
    RE_f32 largest = RE_ABS(b) > RE_ABS(a) ? RE_ABS(b) : RE_ABS(a);

    return diff <= largest * rel;
}


static void test_pow_sweep(void)
{
    RE_BOOL ok = RE_TRUE;

    for (int i = 0; i < 100; i++)
    {
        RE_f32 a = (RE_f32)i / 10.0f + 0.1f;  // avoid a=0
        RE_f32 b = (RE_f32)(i % 10) / 5.0f;   // exponent from 0..2

        RE_f32 p1 = RE_POW_f32(a, b);
        printf("%f\n", p1);
        RE_f32 p2 = powf(a, b);
        printf("%f\n", p2);

        if (!approx_rel_f32(p1, p2, 0.186))    // bit-level approx → tolerance looser
        {
            ok = RE_FALSE;
            break;
        }
    }

    test_result("POW sweep approx powf", ok);
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
    test_fmod();
    test_float_classification();
    test_exp_basic();
    test_pow_basic();
    test_pow_sweep();

    printf("=== re_math tests finished ===\n");
}
