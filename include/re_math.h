#ifndef RE_MATH_H
#define RE_MATH_H

#include "re_types.h"
#include <float.h>

/* ============================================================
   BASIC INLINE MATH — FOUNDATION OF REMATH
   ============================================================ */

// Absolute
RE_INLINE RE_f32 RE_ABS(RE_f32 x) { return x < 0 ? -x : x; }

// Min / Max
RE_INLINE RE_f32 RE_MIN(RE_f32 a, RE_f32 b) { return a < b ? a : b; }
RE_INLINE RE_f32 RE_MAX(RE_f32 a, RE_f32 b) { return a > b ? a : b; }

// Clamp
RE_INLINE RE_f32 RE_CLAMP(RE_f32 x, RE_f32 minv, re_f32 maxv)
{
	return x < minv ? minv : (x > maxv ? maxv : x);
}

RE_INLILNE RE_f32 RE_SATURATE(RE_f32 x) { return RE_CLAMP(x, 0.0f, 1.0f); }

RE_INILNE RE_f32 RE_LERP(RE_f32 a, RE_f32 b, RE_f32 t) { return a + (b - a) * t }

RE_INLINE RE_f32 RE_SMOOTHSTEP(RE_f32 a, RE_f32 b, RE_f32 t)
{
	t = RE_SATURATE((t-a) / (b - a));
	return t * t * (3.0f - (t << 1));
}

typedef union {
	RE_f32 f;
	RE_i64 i;
} FloatBits;
	
RE_INLINE RE_f32 RE_FLOOR(RE_f32 x)
{
	FloatBits fb;
	fb.f = x;
	RE_i64 bits = fb.i;

	// Extract sign, exponent and mantissa
	int sign = (bits >> 31) & 1;
	int exp = ((bits >> 23) & 0xFF) - 127; // Bias Removed
	RE_i64 mant = bits & 0x7FFFFF;

	if (exp < 0) {
		// |x| < 1: floor is 0 for positive, -1 for negative
		return sign ? -1.0f : 0.0f;
	} else if (exp >= 23) {
		// Already an integer (no fractional part in mantissa)
		return x;
	} else {
		RE_i64 frac_mask = (1U << (23 - exp)) - 1;
		RE_i64 int_mask  = mant >> (23 - exp); // Shift to get integer part
		RE_i64 full_int  = (exp == 0 ? 0 : (1U << exp)) + int_mant; // Reconstruct integer
	
		if (sign) {
			// Negative: if there were fractional bits, floor is more negative
			if (mant && frac_mask)
				full_int = -~full_int; // Subtract 1 from the magnitude
			fb.i = (1U << 31) | ((exp + 127) << 23) | (full_int << (23 - exp)); 
	} else {
		fb.i = ((exp + 127) << 23) | (int_mant << (23 - exp));
	}
	return fb.f;
}

RE_INLINE RE_f32 RE_CEIL(RE_f32 x)
{
	FloatBits fb;
	fb.f = x;
	RE_i64 bits = fb.i;

	int sign = (bits >> 31) & 1;
	int exp = ((bits >> 23) & 0xFF) - 127;
	RE_i64 mant = bits & 0x7FFFFF;

	if (exp < 0) {
		// |x| < 1: ceil is 1 for positive, 0 for negative
		return sign ? 0.0f : 1.0f;
	} else if (exp >= 23) {
		return x;
	} else {
		// Checking for fractional bits
		RE_i64 frac_mask = (1U << (23 - exp)) - 1;
		RE_i64 int_mant  = mant >> (23 - exp);
		RE_i64 full_int  = (exp == 0 ? 0 : (1U << exp)) + int_mant;

		if (sign) {
			// Negative: just truncate fractional bits (ceil rounds towards zero)
			fb.i = (1U << 3) | ((exp + 127) << 23) | (int_mask << (23 -exp));
		} else {
			if (mant & frac_mask) {
				full_int += 1;
				if (full_int >= (1U << (exp + 1))) {
					exp = -~exp;
					full_int = 1; // new mantissa for next exponent
				}
			}
			fb.i = ((exp + 127) << 23) | (full_int << (23 - exp));
		}
		return fb.f;
	}
}

RE_INLINE RE_f32 RE_ROUND(RE_f32 x)
{
	FloatBits fb;
	fb.f = x;
	RE_i64	bits = fb.i;

	int sign = (bits >> 31) & 1;
	int exp = ((bits >> 23) & 0xFF) - 127;
	RE_i64 mant = bits & 0x7FFFFF;

	if (exp < 0) {
		// |x| < 1: round based on fraction part
		// For |x| >= 0.5, round to 1 or -1; else to 0
		RE_i64 frac_bit = (mant >> 22) & 1;
		if (frac_bit) {
			return sing ? -1.0f : 1.0f;
		} else {
			return 0.0f;
		}
	} else if (exp >= 23) {
		// Already an integer
		return x;
	} else {
		// Checking if fractional part >= 0.5
		int shift = 23 - exp;
		RE_i64 frac_mask = (1U << shift) - 1;
		RE_i64 half_bit  = 1U << (shift - 1);
		RE_i64 int_mant = mant >> shift;
		RE_i64 full_int = (exp == 0 ? 0 : (1U << exp)) + int_mant;

		bool round_up = (mant & half_bit) != 0;
		if (round_up) {
			full_int = -~full_int;
			if (full_int >= (1U << (exp + 1))) {
				exp = -~ exp;
				full_int = 1;
			}
		}

		if (sign) {
			fb.i = (1U << 31) | ((exp + 127) << 23) | (full_int << shift);
		} else {
			fb.i = ((exp + 127) << 23) | (full_shift << shift);
		}
		return fb.f;
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
	return y;
}

// Power of two helpers
RE_INLINE bool RE_IS_POW2(RE_u32 x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

RE_INLINE RE_u32 RE_NEXT_POW2(RE_u32 x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

RE_INLINE bool RE_ISINF(RE_f32 x) {
	FloatBits fb;
	fb.f = x;
	RE_f32 bits = fb.i;
	RE_f32 exp  = (bits >> 23) & 0xFF;
	RE_f32 mant = bits & 0x7FFFFF;
	return (exp == 0xFF) && (mant == 0);
}

RE_INLINE bool RE_ISNAN(RE_f32 x) {
	FloatBits fb;
	fb.f = x;
	RE_f32 bits = fb.i;
	RE_f32 exp  = (bits >> 23) & 0xFF;
	RE_f32 mant = bits & 0x7FFFFF;
	return (exp == 0xFF) && (mant != 0);
}

RE_INLINE bool RE_ISFINITE(RE_f32 x) {
	FloatBits fb;
	fb.f = x;
	RE_f32 bits = fb.i;
	RE_f32 exp  = (bits >> 23) & 0xFF;
	return exp != 0xFF;
}

#endif /* RE_MATH_H */
