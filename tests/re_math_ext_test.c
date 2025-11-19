/**
 * @file re_math_ext_test.c
 * @brief Unit tests for REMath extended utilities (angles, remap, RNG, fast trig).
 *
 * This suite verifies correctness and stability of all inline utilities
 * in `re_math_ext.h`, including:
 *   - angle helpers
 *   - remap / smooth / snap / deadzone
 *   - fast inverse sqrt / sqrt
 *   - fast sin/cos polynomial approximations
 *   - fast atan2
 *   - hash->float, RNG floats, random unit vectors
 *
 * @author
 * Jayansh Devgan
 */

#include "../include/re_math_ext.h"
#include "../include/re_core.h"
#include "../include/re_test_core.h"

#include <math.h>
#include <stdio.h>

/* ============================================================================================
   TEST HELPERS
   ============================================================================================ */

static RE_bool approx_eq_f32(RE_f32 a, RE_f32 b, RE_f32 eps)
{
    return RE_ABS(a - b) <= eps;
}

/* ============================================================================================
   TEST: Angle helpers
   ============================================================================================ */

static void test_angle_helpers(void)
{
    test_result("DEG2RAD 180",  approx_eq_f32(RE_DEG2RAD_f32(180.f), RE_PI_F, 1e-6f));
    test_result("RAD2DEG PI",   approx_eq_f32(RE_RAD2DEG_f32(RE_PI_F), 180.f, 1e-6f));

    RE_f32 w1 = RE_WRAP_ANGLE_RAD_f32(3.5f * RE_PI_F);
    test_result("WRAP angle > PI",
        w1 > -RE_PI_F && w1 <= RE_PI_F
    );

    RE_f32 w2 = RE_WRAP_ANGLE_RAD_f32(-4.2f * RE_PI_F);
    test_result("WRAP angle < -PI",
        w2 > -RE_PI_F && w2 <= RE_PI_F
    );
}

/* ============================================================================================
   TEST: Remap / Snap / Smooth / Deadzone
   ============================================================================================ */

static void test_remap_snap_smooth(void)
{
    test_result("REMAP mid",
        approx_eq_f32(RE_REMAP_f32(5.f, 0.f, 10.f, 0.f, 1.f), 0.5f, 1e-6f));

    test_result("REMAP_CLAMPED low",
        approx_eq_f32(RE_REMAP_CLAMPED_f32(-1.f, 0.f, 10.f, 0.f, 1.f), 0.f, 1e-6f));

    test_result("SNAP",
        approx_eq_f32(RE_SNAP_f32(5.3f, 1.f), 5.f, 1e-6f));

    /* Smoothstep */
    test_result("SMOOTHSTEP edge0",
        approx_eq_f32(RE_SMOOTHSTP_f32(0.f, 1.f, 0.f), 0.f, 1e-6f));

    test_result("SMOOTHSTEP edge1",
        approx_eq_f32(RE_SMOOTHSTP_f32(0.f, 1.f, 1.f), 1.f, 1e-6f));

    /* Smootherstep */
    RE_f32 sm = RE_SMOOTHERSTEP_f32(0.f, 1.f, 0.5f);
    test_result("SMOOTHERSTEP mid", sm > 0.3f && sm < 0.7f);

    /* Deadzone */
    test_result("DEADZONE inside", RE_DEADZONE_f32(0.1f, 0.2f) == 0.f);
    test_result("DEADZONE outside",
        approx_eq_f32(RE_DEADZONE_f32(0.5f, 0.2f), 0.5f, 1e-6f));

    /* Deadzone normalized */
    RE_f32 dn = RE_DEADZONE_NORM_f32(0.5f, 0.2f);
    test_result("DEADZONE_NORM", dn > 0.f);
}

/* ============================================================================================
   TEST: Fast inv sqrt & sqrt
   ============================================================================================ */

static void test_fast_invsqrt_sqrt(void)
{
    RE_f32 x = 9.f;

    RE_f32 inv_fast = RE_INV_SQRT_FAST_f32(x);
    RE_f32 inv_ref  = 1.f / sqrtf(x);

    test_result("INV_SQRT_FAST approx", approx_eq_f32(inv_fast, inv_ref, 0.02f));

    RE_f32 s_fast = RE_SQRT_FAST_f32(x);
    test_result("SQRT_FAST approx", approx_eq_f32(s_fast, 3.f, 0.02f));
}

/* ============================================================================================
   TEST: Fast sin/cos and sincos
   ============================================================================================ */

static void test_fast_sincos(void)
{
    RE_f32 s, c;

    RE_FAST_SINCOS_f32(RE_PI_F * 0.5f, &s, &c);

    test_result("FAST_SIN approx", approx_eq_f32(s, 1.f, 1e-3f));
    test_result("FAST_COS approx", approx_eq_f32(c, 0.f, 1e-3f));

    RE_f32 ss = RE_SIN_f32(RE_PI_F);
    RE_f32 cc = RE_COS_f32(0.f);

    test_result("SIN(pi) approx", approx_eq_f32(ss, 0.f, 1e-3f));
    test_result("COS(0) approx", approx_eq_f32(cc, 1.f, 1e-3f));
}

/* ============================================================================================
   TEST: Fast Atan2
   ============================================================================================ */

static void test_fast_atan2(void)
{
    RE_f32 a = RE_ATAN2_f32(1.f, 0.f);
    test_result("ATAN2(y=1,x=0)", approx_eq_f32(a, RE_PI_F * 0.5f, 1e-3f));

    RE_f32 b = RE_ATAN2_f32(0.f, -1.f);
    test_result("ATAN2(y=0,x=-1)", approx_eq_f32(b, RE_PI_F, 1e-2f));
}

/* ============================================================================================
   TEST: Hash -> float, RNG, unit vectors
   ============================================================================================ */

static void test_hash_rng(void)
{
    /* Hash → float */
    RE_u32 h = RE_HASH_u32(12345);
    RE_f32 hf = RE_HASH_TO_f32(h);

    test_result("HASH_TO_f32 range", hf >= 0.f && hf < 1.f);

    /* RNG */
    RE_RNG32 rng = { 123456u };

    RE_f32 r1 = RE_RNG32_NEXT_f32(&rng);
    test_result("RNG_NEXT_f32 range", r1 >= 0.f && r1 < 1.f);

    RE_f32 r2 = RE_RNG32_RANGE_f32(&rng, -5.f, 5.f);
    test_result("RNG_RANGE_f32 range", r2 >= -5.f && r2 < 5.f);
}

/* ============================================================================================
   TEST: Random unit vectors
   ============================================================================================ */

static void test_random_unit_vectors(void)
{
    RE_RNG32 rng = { 99999u };

    /* 2D */
    RE_f32 x, y;
    RE_RAND_UNIT_VEC2_f32(&rng, &x, &y);

    RE_f32 len2 = x*x + y*y;
    test_result("UNIT_VEC2 approx", approx_eq_f32(len2, 1.f, 0.02f));

    /* 3D */
    RE_f32 x3, y3, z3;
    RE_RAND_UNIT_VEC3_f32(&rng, &x3, &y3, &z3);

    RE_f32 l3 = x3*x3 + y3*y3 + z3*z3;
    test_result("UNIT_VEC3 approx", approx_eq_f32(l3, 1.f, 0.02f));
}

/* ============================================================================================
   RUN ALL TESTS
   ============================================================================================ */

void run_math_ext_tests(void)
{
    printf("=== re_math_ext tests start ===\n");

    test_angle_helpers();
    test_remap_snap_smooth();
    test_fast_invsqrt_sqrt();
    test_fast_sincos();
    test_fast_atan2();
    test_hash_rng();
    test_random_unit_vectors();

    printf("=== re_math_ext tests finished ===\n");
}
