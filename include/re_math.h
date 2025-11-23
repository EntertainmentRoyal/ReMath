/**
 * @file re_math.h
 * @brief Core mathematical helper functions for REMath.
 *
 * These are the foundational low-level math utilities that power all
 * higher-level vector, matrix, quaternion, and geometry operations.
 *
 * @author
 * Jayansh Devgan
 */

#ifndef RE_MATH_H
#define RE_MATH_H

#include "re_core.h"
#include "re_constants.h"

#include <float.h>
#include <string.h>

/**
 * @defgroup REMath_Core
 * @brief Basic mathematical operations and bit-level float manipulation.
 * @{
 */

/* ============================================================
   BASIC INLINE MATH — FOUNDATION OF REMATH
   ============================================================ */

// Absolute
RE_INLINE RE_f32 RE_ABS(RE_f32 x) { return x < 0 ? -x : x; }

// Min / Max
RE_INLINE RE_f32 RE_MIN(RE_f32 a, RE_f32 b) { return a < b ? a : b; }
RE_INLINE RE_f32 RE_MAX(RE_f32 a, RE_f32 b) { return a > b ? a : b; }

// Clamp
RE_INLINE RE_f32 RE_CLAMP(RE_f32 x, RE_f32 minv, RE_f32 maxv)
{
	return x < minv ? minv : (x > maxv ? maxv : x);
}

RE_INLINE RE_f32 RE_CLAMP01(RE_f32 x) {
    RE_f32 t = x < 0.0f ? 0.0f : x;
    return t > 1.0f ? 1.0f : t;
}

RE_INLINE RE_f32 RE_LERP(RE_f32 a, RE_f32 b, RE_f32 t) { return a + (b - a) * t; }

RE_INLINE RE_f32 RE_SMOOTHSTEP(RE_f32 a, RE_f32 b, RE_f32 t)
{
	t = RE_CLAMP01((t-a) / (b - a));
	RE_i32 t_bits = *(RE_i32*)&t;
	t_bits += (1 << 23);
	RE_f32 two_t = *(RE_f32*)&t_bits;
	return t * t * (3.0f - two_t);
}

typedef union {
	RE_f32 f;
	RE_i64 i;
} FloatBits;

RE_INLINE RE_f32 RE_FLOOR(RE_f32 x)
{
    FloatBits fb;
       fb.f = x;
       RE_u32 bits = fb.i;

       RE_i32 sign = (bits >> 31) & 1;
       RE_i32 exp  = ((bits >> 23) & 0xFF) - 127;
       RE_u32 mant = bits & 0x7FFFFF;

       // |x| < 1
       if (exp < 0) {
           if (sign && x != 0.0f)
               return -1.0f;
           return 0.0f;
       }

       // No fractional part
       if (exp >= 23) {
           return x;
       }

       // Mask fractional bits
       RE_u32 frac_mask = (1U << (23 - exp)) - 1;

       // No fractional bits => already integer
       if ((mant & frac_mask) == 0) {
           return x;
       }

       RE_u32 new_mant = mant & ~frac_mask;

       if (!sign) {
           fb.i = (bits & 0xFF800000) | new_mant;
           return fb.f;
       }

       // Negative: truncation moves toward zero, we must go one step more negative
       // → subtract 1.0 in float domain using bit logic

       // Reconstruct truncated value first
       fb.i = (bits & 0xFF800000) | new_mant;
       RE_f32 truncated = fb.f;

       // Floor(x) = truncated - 1.0 for negative values with fractional parts
       // Implement subtracting 1.0 using float bits:
       FloatBits fb2;
       fb2.f = truncated;

       fb2.f = truncated - 1.0f;

       return fb2.f;
 }

RE_INLINE RE_f32 RE_CEIL(RE_f32 x)
{
    FloatBits fb;
        fb.f = x;
        RE_u32 bits = fb.i;

        RE_i32 sign = (bits >> 31) & 1;
        RE_i32 exp  = ((bits >> 23) & 0xFF) - 127;
        RE_u32 mant = bits & 0x7FFFFF;

        // |x| < 1
        if (exp < 0)
        {
            return sign ? 0.0f : 1.0f;
        }

        // No fractional bits => already integer
        if (exp >= 23)
            return x;

        // Mask fractional bits
        RE_u32 frac_mask = (1u << (23 - exp)) - 1;

        // No fractional part => already exact integer
        if ((mant & frac_mask) == 0)
            return x;

        // Zero-out fractional bits to get truncated value
        RE_u32 new_mant = mant & ~frac_mask;
        fb.i = (bits & 0xFF800000) | new_mant;
        RE_f32 truncated = fb.f;

        if (!sign)
        {
            // positive: ceil(x) = truncated + 1.0
            truncated += 1.0f;
        }
        else
        {
            // negative: ceil(x) = truncated (toward zero)
            // e.g. -3.7 → -3
        }

        return truncated;
}

RE_INLINE RE_f32 RE_ROUND(RE_f32 x)
{
    FloatBits fb;
    fb.f = x;
    RE_u32 bits = fb.i;

    RE_i32 sign = bits >> 31;
    RE_i32 exp = ((bits >> 23) & 0xFF) - 127;
    RE_u32 mant = bits & 0x7FFFFF;

    // |x| < 1: directly decide using midpoint (0.5f)
    if (exp < 0)
    {
        // fractional part = x itself
        if (x > 0.5f) return 1.0f;
        if (x < -0.5f) return -1.0f;
        return 0.0f;
    }

    // Already an integer
    if (exp >= 23)
        return x;

    RE_u32 shift = 23 - exp;
    RE_u32 frac_mask = (1u << shift) - 1;
    RE_u32 int_mask = mant >> shift;

    // truncated integer magnitude
    RE_f32 truncated;
    {
        fb.i = (bits & 0xFF800000) | (mant & ~frac_mask);
        truncated = fb.f;
    }

    // fractional part exists?
    RE_u32 frac = mant & frac_mask;
    if (frac == 0)
        return truncated; // exact

    // half-way threshold: 0.5 = bit (shift-1)
    RE_u32 half_bit = 1u << (shift - 1);

    RE_BOOL round_up = (frac >= half_bit);

    if (!round_up)
        return truncated;

    // rounding direction: increase magnitude
    if (sign)
    {
        // negative: rounded = truncated - 1
        return truncated - 1.0f;
    }
    else
    {
        // positive: truncated + 1
        return truncated + 1.0f;
    }
}

// Fractional part
RE_INLINE RE_f32 RE_FRAC(RE_f32 x) { return x - RE_FLOOR(x); }

// Sign(returns -1, 0 or 1)
RE_INLINE RE_f32 RE_SIGN(RE_f32 x) {
	if (x > 0) return 1.0f;
	if (x < 0) return -1.0f;
	return 0.0f;
}

// Fast inverse sqaure root (Quake III style)
RE_INLINE RE_f32 RE_INVSQRT(RE_f32 number) {
	long i;
	RE_f32 x2, y;
	const RE_f32 threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = *(long*)&y;			// Evil floating point bit hacking
	i = 0x5f3759df - (i >> 1);	// Magic Number
	y = *(RE_f32*)&i;
	y = y * (threehalfs - (x2 * y * y));
	y = y * (threehalfs - (x2 * y * y)); // Second refinement iteration (added for higher accuracy)
	return y;
}

/**
 * @brief Fast square root using inverse square root + one Newton refinement.
 */
RE_INLINE RE_f32 RE_SQRT(RE_f32 number) {
    if (number <= 0.0f) return 0.0f;

    RE_f32 inv = RE_INVSQRT(number);

    // sqrt(x) = x * inv_sqrt(x)
    RE_f32 y = number * inv;

    // One Newton refinement for sqrt
    // y = 0.5 * (y + x / y)
    y = 0.5f * (y + number / y);

    return y;
}


// Power of two helpers
RE_INLINE RE_BOOL RE_IS_POW2(RE_u32 x) { return (x != 0) && ((x & (x - 1)) == 0); }

RE_INLINE RE_u32 RE_NEXT_POW2(RE_u32 x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

RE_INLINE RE_BOOL RE_ISINF(RE_f32 x) {
	FloatBits fb;
	fb.f = x;
	RE_u32 bits = fb.i;
	RE_u32 exp  = (bits >> 23) & 0xFF;
	RE_u32 mant = bits & 0x7FFFFF;
	return (exp == 0xFF) && (mant == 0);
}

RE_INLINE RE_BOOL RE_ISNAN(RE_f32 x) {
	FloatBits fb;
	fb.f = x;
	RE_u32 bits = fb.i;
	RE_u32 exp  = (bits >> 23) & 0xFF;
	RE_u32 mant = bits & 0x7FFFFF;
	return (exp == 0xFF) && (mant != 0);
}

RE_INLINE RE_BOOL RE_ISFINITE(RE_f32 x) {
	FloatBits fb;
	fb.f = x;
	RE_u32 bits = fb.i;
	RE_u32 exp  = (bits >> 23) & 0xFF;
	return exp != 0xFF;
}

RE_INLINE RE_f32 RE_LOG2_f32(RE_f32 x)
{
    union { RE_f32 f; RE_u32 i; } v = { x };

    RE_i32 ex = (v.i >> 23) & 255;
    RE_f32 e  = (RE_f32)ex - 127.0f;

    v.i = (v.i & 0x7FFFFF) | (127 << 23);
    RE_f32 m = v.f;

    // Polynomial: log2(m) ≈ p(m)
    RE_f32 t = m - 1.0f;
    RE_f32 p = t * (1.4426950408889634f +
            t * (-0.7213475204444817f +
            t * (0.480898346962987f)));

    return e + p;
}

RE_INLINE RE_f32 RE_EXP_f32(RE_f32 x)
{
    if (x > 88.0f)  return 3.402823e38f;
        if (x < -88.0f) return 0.0f;

        const RE_f32 inv_ln2 = 1.4426950408889634f;
        RE_f32 fx = x * inv_ln2;

        RE_i32 ix = (RE_i32)fx;
        if (fx < 0.0f) ix--;

        RE_f32 f = fx - (RE_f32)ix;

        RE_f32 p =
               1.0f +
            f *(0.69314718f +
            f *(0.24022651f +
            f *(0.05550411f +
            f *(0.00961813f +
            f *0.00133336f))));

        RE_i32 e = (ix + 127) << 23;

        RE_f32 pow2i;
        memcpy(&pow2i, &e, sizeof(e));

        return pow2i * p;
}

RE_INLINE RE_f32 RE_POW_f32(RE_f32 a, RE_f32 b)
{
    if (a <= 0.0f) return 0.0f;

    if (b == 0.0f) return 1.0f;
    if (b == 1.0f) return a;
    if (b == 2.0f) return a * a;
    if (b == 0.5f) return RE_SQRT(a);
    if (b == -1.0f) return 1.0f / a;

    RE_f32 ln_a = RE_LOG2_f32(a) * RE_LN2_F;

    RE_f32 t = b * ln_a;
    return RE_EXP_f32(t);
}

RE_INLINE RE_f32 RE_FMOD_f32(RE_f32 x, RE_f32 y)
{
    if (y == 0.0f)
        return 0.0f;

    RE_f32 r = x - (RE_f32)((RE_i32)(x / y)) * y;

    if ((r != 0.0f) && ((r < 0.0f) != (y < 0.0f)))
        r += y;

    return r;
}

RE_INLINE RE_f32 RE_FMAX_f32(RE_f32 a, RE_f32 b)
{
    return (a > b) ? a : b;
}

RE_INLINE RE_f32 RE_FMIN_f32(RE_f32 a, RE_f32 b)
{
    return (a < b) ? a : b;
}


#endif /* RE_MATH_H */
