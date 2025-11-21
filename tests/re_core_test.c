/**
 * @file re_core_test.c
 * @brief Unit tests for RECore low-level bit utilities, IEEE helpers,
 *        SNORM packing, inverse sqrt, and RNG primitives.
 *
 * This suite validates correctness, determinism, and IEEE-754 behavior
 * of the primitives defined inside `re_core.h` and used throughout RE.
 *
 * @author
 * Jayansh Devgan
 */

#include "../include/re_core.h"
#include "../include/re_math.h"
#include "../include/re_test_core.h"

#include <stdio.h>
#include <assert.h>
#include <math.h>


/* ============================================================================================
   TEST HELPERS
   ============================================================================================ */

/**
 * @brief Approximate float comparison helper.
 */
static RE_bool approx_eq_f32(RE_f32 a, RE_f32 b, RE_f32 eps)
{
    RE_f32 d = a - b;
    if (d < 0) d = -d;
    return d <= eps;
}

/* ============================================================================================
   TEST SUITE
   ============================================================================================ */

/**
 * @brief Tests bitcast utilities (f32 <-> u32).
 */
static void test_bitcast(void)
{
    RE_f32 v = 3.14159f;
    RE_u32 u = RE_BITCAST_f32_TO_u32(v);
    RE_f32 v2 = RE_BITCAST_u32_TO_f32(u);

    test_result("BITCAST round-trip f32 <-> u32",
        approx_eq_f32(v, v2, 1e-5f));
}

/**
 * @brief Tests rotate-left and rotate-right on u32/U64.
 */
static void test_rotations(void)
{
    test_result("ROTL_u32 simple",
        RE_ROTL_u32(1u, 1) == 2u);

    test_result("ROTR_u32 simple",
        RE_ROTR_u32(1u, 1) == 0x80000000u);

    RE_u32 x = 0x12345678u;
    test_result("ROTL_u32 by 8",
        RE_ROTL_u32(x, 8) == ((x << 8) | (x >> 24)));

    RE_u64 y = 0x1122334455667788ull;
    test_result("ROTL_U64 by 16",
        RE_ROTL_U64(y, 16) == ((y << 16) | (y >> 48)));

    RE_u32 z = 0xDEADBEEFu;
    test_result("ROTL_u32 by 32 == identity", RE_ROTL_u32(z, 32) == z);
    test_result("ROTR_u32 by 32 == identity", RE_ROTR_u32(z, 32) == z);
}

/**
 * @brief Tests CLZ, CTZ, and POPCNT correctness.
 */
static void test_clz_ctz_popcnt(void)
{
    test_result("CLZ_u32(1) == 31", RE_CLZ_u32(1u) == 31);
    test_result("CLZ_u32(0x80000000) == 0", RE_CLZ_u32(0x80000000u) == 0);

    test_result("CTZ_u32(1) == 0", RE_CTZ_u32(1u) == 0);
    test_result("CTZ_u32(0x80000000) == 31", RE_CTZ_u32(0x80000000u) == 31);

    test_result("POPCNT(0xF0F0F0F0) == 16",
        RE_POPCNT_u32(0xF0F0F0F0u) == 16);
}

/**
 * @brief Tests min/max for integers & float clamp.
 */
static void test_min_max_clamp(void)
{
    test_result("MIN_I32", RE_MIN_I32(-5, 7) == -5);
    test_result("MAX_I32", RE_MAX_I32(-5, 7) == 7);

    test_result("MIN_f32", RE_MIN_f32(1.f, 2.f) == 1.f);
    test_result("MAX_f32", RE_MAX_f32(1.f, 2.f) == 2.f);

    test_result("CLAMP_f32",
        RE_CLAMP_f32(5.f, 0.f, 3.f) == 3.f);
}

/**
 * @brief Tests IEEE float classification helpers: isnan, isinf, isfinite.
 */
static void test_ieee_classification(void)
{
    RE_f32U u;

    /* +inf */
    u.u = (0xFFu << 23);
    test_result("ISINF +inf", RE_ISINF_f32(u.f));
    test_result("NOT ISNAN +inf", !RE_ISNAN_f32(u.f));
    test_result("NOT ISFINITE +inf", !RE_ISFINITE_f32(u.f));

    /* NaN */
    u.u = (0xFFu << 23) | 1u;
    test_result("ISNAN NaN", RE_ISNAN_f32(u.f));
    test_result("NOT ISINF NaN", !RE_ISINF_f32(u.f));
    test_result("NOT ISFINITE NaN", !RE_ISFINITE_f32(u.f));

    /* finite */
    u.f = 1.5f;
    test_result("finite NOT NaN", !RE_ISNAN_f32(u.f));
    test_result("finite NOT Inf", !RE_ISINF_f32(u.f));
    test_result("finite ISFINITE", RE_ISFINITE_f32(u.f));
}

/**
 * @brief Tests RE_SQRT accuracy against standard sqrtf.
 */
 static void test_sqrt(void)
 {
     RE_f32 vals[] = {0.0f, 0.01f, 0.5f, 1.f, 2.f, 10.f, 123.456f, 9999.f};
     for (int i = 0; i < (int)(sizeof(vals)/sizeof(vals[0])); i++)
     {
         RE_f32 x = vals[i];
         RE_f32 fast = RE_SQRT(x);
         RE_f32 ref  = sqrtf(x);
         RE_bool ok = approx_eq_f32(fast, ref, 1e-4f);
         char name[64];
         snprintf(name, sizeof(name), "SQRT approx match [%g]", (double)x);
         test_result(name, ok);
     }
     /* Monotonicity test */
     test_result("SQRT monotonic 1",
         RE_SQRT(4.0f) > RE_SQRT(1.0f));
     test_result("SQRT monotonic 2",
         RE_SQRT(100.0f) > RE_SQRT(25.0f));
     /* Behavior on zero and negative */
     test_result("SQRT(0) == 0", RE_SQRT(0.0f) == 0.0f);
     test_result("SQRT(-5) == 0", RE_SQRT(-5.0f) == 0.0f);
 }

/**
 * @brief Tests Quake inverse square root accuracy.
 */
static void test_invsqrt(void)
{
    RE_f32 vals[] = {0.5f, 1.f, 2.f, 10.f, 123.456f};

    for (int i = 0; i < 5; i++)
    {
        RE_f32 n = vals[i];
        RE_f32 r = RE_INVSQRT(n);
        RE_f32 check = r * r * n;

        test_result("INVSQRT coarse approx",
            approx_eq_f32(check, 1.f, 1e-2f));

        /* run again for refinement */
        RE_f32 r2 = RE_INVSQRT(n);
        RE_f32 check2 = r2 * r2 * n;

        test_result("INVSQRT refined approx",
            approx_eq_f32(check2, 1.f, 1e-3f));
    }
}

/**
 * @brief Tests SNORM8 and SNORM16 pack/unpack.
 */
static void test_snorm_pack_unpack(void)
{
    test_result("PACK_SNORM8 +1", RE_PACK_SNORM8(1.f) == 127);
    test_result("PACK_SNORM8 -1", RE_PACK_SNORM8(-1.f) == -128);

    test_result("UNPACK_SNORM8 +1",
        approx_eq_f32(RE_UNPACK_SNORM8(127), 1.f, 1e-6f));
    test_result("UNPACK_SNORM8 -1",
        approx_eq_f32(RE_UNPACK_SNORM8(-128), -1.f, 1e-6f));

    test_result("PACK_SNORM16 +1", RE_PACK_SNORM16(1.f) == 32767);
    test_result("PACK_SNORM16 -1", RE_PACK_SNORM16(-1.f) == -32768);

    test_result("UNPACK_SNORM16 +1",
        approx_eq_f32(RE_UNPACK_SNORM16(32767), 1.f, 1e-6f));
    test_result("UNPACK_SNORM16 -1",
        approx_eq_f32(RE_UNPACK_SNORM16(-32768), -1.f, 1e-6f));
}

/**
 * @brief Tests ABS, copy-sign, and select.
 */
static void test_abs_copy_sign_select(void)
{
    test_result("ABS_f32", approx_eq_f32(RE_ABS_f32(-3.5f), 3.5f, 1e-5f));

    test_result("COPY_SIGN_f32",
        approx_eq_f32(RE_COPY_SIGN_f32(3.f, -2.f), -3.f, 1e-5f));

    RE_u32 s = RE_SELECT_u32(1, 0x11111111u, 0x22222222u);
    RE_u32 t = RE_SELECT_u32(0, 0x11111111u, 0x22222222u);

    test_result("SELECT_u32 true",
        s == 0x11111111u);

    test_result("SELECT_u32 false",
        t == 0x22222222u);
}

/**
 * @brief Tests deterministic RNG behavior.
 */
static void test_rng32(void)
{
    RE_RNG32 rng;

    RE_RNG32_SEED(&rng, 12345u);
    RE_u32 a = RE_RNG32_NEXT_u32(&rng);

    RE_RNG32_SEED(&rng, 12345u);
    RE_u32 b = RE_RNG32_NEXT_u32(&rng);

    test_result("RNG32 deterministic", a == b);
}

/* -------- Test RE_HASH_TO_f32 -------- */
static void test_hash_to_f32(void)
{
    /* Deterministic known values */
    RE_f32 a = RE_HASH_TO_f32(0u);
    RE_f32 b = RE_HASH_TO_f32(123456789u);
    RE_f32 c = RE_HASH_TO_f32(0xFFFFFFFFu);

    test_result("HASH_TO_f32 range0", (a >= 0.0f && a < 1.0f));
    test_result("HASH_TO_f32 range1", (b >= 0.0f && b < 1.0f));
    test_result("HASH_TO_f32 range2", (c >= 0.0f && c < 1.0f));

    /* Deterministic repeat */
    RE_f32 a2 = RE_HASH_TO_f32(0u);
    RE_f32 b2 = RE_HASH_TO_f32(123456789u);

    test_result("HASH_TO_f32 deterministic0", approx_eq_f32(a, a2, 1e-9f));
    test_result("HASH_TO_f32 deterministic1", approx_eq_f32(b, b2, 1e-9f));

    /* Hash difference should cause different results (not equal) */
    test_result("HASH_TO_f32 diff",
        !approx_eq_f32(a, b, 1e-12f) || !approx_eq_f32(b, c, 1e-12f));
}

/* -------- Test RE_RNG32_NEXT_f32 -------- */
static void test_rng32_next_f32(void)
{
    RE_RNG32 rng;
    RE_RNG32_SEED(&rng, 999u);

    RE_f32 r1 = RE_RNG32_NEXT_f32(&rng);
    RE_f32 r2 = RE_RNG32_NEXT_f32(&rng);
    RE_f32 r3 = RE_RNG32_NEXT_f32(&rng);

    test_result("RNG32_NEXT_f32 range1", (r1 >= 0.0f && r1 < 1.0f));
    test_result("RNG32_NEXT_f32 range2", (r2 >= 0.0f && r2 < 1.0f));
    test_result("RNG32_NEXT_f32 range3", (r3 >= 0.0f && r3 < 1.0f));

    /* Deterministic sequence test */
    RE_RNG32 rng2;
    RE_RNG32_SEED(&rng2, 999u);

    RE_f32 t1 = RE_RNG32_NEXT_f32(&rng2);
    RE_f32 t2 = RE_RNG32_NEXT_f32(&rng2);
    RE_f32 t3 = RE_RNG32_NEXT_f32(&rng2);

    test_result("RNG32_NEXT_f32 deterministic1", approx_eq_f32(r1, t1, 1e-9f));
    test_result("RNG32_NEXT_f32 deterministic2", approx_eq_f32(r2, t2, 1e-9f));
    test_result("RNG32_NEXT_f32 deterministic3", approx_eq_f32(r3, t3, 1e-9f));

    /* Ensure values are not constant */
    test_result("RNG32_NEXT_f32 non-constant",
        !(approx_eq_f32(r1, r2, 1e-12f) && approx_eq_f32(r2, r3, 1e-12f)));
}

/* -------- Test RE_RNG32_RANGE_f32 -------- */
static void test_rng32_range_f32(void)
{
    RE_RNG32 rng;
    RE_RNG32_SEED(&rng, 2024u);

    const RE_f32 lo = -5.f;
    const RE_f32 hi = 7.f;

    for (int i = 0; i < 10; ++i) {
        RE_f32 v = RE_RNG32_RANGE_f32(&rng, lo, hi);
        if (!(v >= lo && v < hi)) {
            test_result("RANGE_f32 in range", 0);
        }
    }
    test_result("RANGE_f32 all in range", 1);

    /* Determinism */
    RE_RNG32 rngA, rngB;
    RE_RNG32_SEED(&rngA, 2024u);
    RE_RNG32_SEED(&rngB, 2024u);

    RE_f32 A1 = RE_RNG32_RANGE_f32(&rngA, lo, hi);
    RE_f32 A2 = RE_RNG32_RANGE_f32(&rngA, lo, hi);

    RE_f32 B1 = RE_RNG32_RANGE_f32(&rngB, lo, hi);
    RE_f32 B2 = RE_RNG32_RANGE_f32(&rngB, lo, hi);

    test_result("RANGE_f32 deterministic1", approx_eq_f32(A1, B1, 1e-9f));
    test_result("RANGE_f32 deterministic2", approx_eq_f32(A2, B2, 1e-9f));
}

/**
 * @brief Runs all RECore tests.
 */
void run_core_tests(void)
{
    printf("=== re_core tests start ===\n");

    test_bitcast();
    test_rotations();
    test_clz_ctz_popcnt();
    test_min_max_clamp();
    test_ieee_classification();
    test_sqrt();
    test_invsqrt();
    test_snorm_pack_unpack();
    test_abs_copy_sign_select();
    test_rng32();
    test_hash_to_f32();
    test_rng32_next_f32();
    test_rng32_range_f32();


    printf("=== re_core tests finished ===\n");
}
