#ifndef RE_CORE_H
#define RE_CORE_H

/* re_core.h
 *
 * Core types and bit-level utilities for REMath.
 * - No <math.h>
 * - Uses only <stdint.h>, <limits.h>, <float.h>
 * - All symbols use RE_ prefix and are inline-friendly.
 *
 * Intended to be included by:
 *   - re_types.h (if you keep that) or directly by other headers.
 * Do NOT duplicate constants present in re_constants.h
 */

#include <stdint.h>
#include <limits.h>
#include <float.h>

/* ---------------------------
   Basic type aliases & bool
   --------------------------- */
typedef float  RE_f32;
typedef double RE_f64;
typedef int32_t  RE_i32;
typedef uint32_t RE_u32;
typedef int64_t  RE_i64;
typedef uint64_t RE_u64;

/* Branchless boolean type */
typedef int RE_bool;
#ifndef RE_TRUE
# define RE_TRUE  1
# define RE_FALSE 0
#endif

/* Inline macro */
#ifndef RE_INLINE
# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define RE_INLINE static inline
# else
#  define RE_INLINE static
# endif
#endif

/* ---------------------------
   Bit reinterpret helpers (safe via union)
   --------------------------- */

typedef union { RE_f32 f; RE_u32 u; RE_i32 i; } RE_f32U;
typedef union { RE_f64 f; RE_u64 u; RE_i64 i; } RE_F64U;

/* Reinterpret float -> uint32, uint32 -> float */
RE_INLINE RE_u32 RE_BITCAST_f32_TO_u32(RE_f32 v) {
    RE_f32U t; t.f = v; return t.u;
}
RE_INLINE RE_f32 RE_BITCAST_u32_TO_f32(RE_u32 v) {
    RE_f32U t; t.u = v; return t.f;
}

/* Reinterpret double <-> uint64 */
RE_INLINE RE_u64 RE_BITCAST_F64_TO_U64(RE_f64 v) {
    RE_F64U t; t.f = v; return t.u;
}
RE_INLINE RE_f64 RE_BITCAST_U64_TO_F64(RE_u64 v) {
    RE_F64U t; t.u = v; return t.f;
}

/* ---------------------------
   Bit rotation (32/64)
   --------------------------- */

#if defined(__GNUC__) || defined(__clang__)
# define RE_HAVE_BUILTIN_ROTL 1
#endif

RE_INLINE RE_u32 RE_ROTL_u32(RE_u32 x, int r) {
#if defined(RE_HAVE_BUILTIN_ROTL)
    return (x << r) | (x >> (32 - r));
#else
    r &= 31;
    return (x << r) | (x >> ((32 - r) & 31));
#endif
}
RE_INLINE RE_u32 RE_ROTR_u32(RE_u32 x, int r) {
    r &= 31;
    return (x >> r) | (x << ((32 - r) & 31));
}

RE_INLINE RE_u64 RE_ROTL_U64(RE_u64 x, int r) {
    r &= 63;
    return (x << r) | (x >> ((64 - r) & 63));
}
RE_INLINE RE_u64 RE_ROTR_U64(RE_u64 x, int r) {
    r &= 63;
    return (x >> r) | (x << ((64 - r) & 63));
}

/* ---------------------------
   Count-leading-zeros / trailing / popcnt
   wrappers (use builtins when available)
   --------------------------- */

#if defined(__GNUC__) || defined(__clang__)
RE_INLINE int RE_CLZ_u32(RE_u32 x)  { return x ? __builtin_clz(x)  : 32; }
RE_INLINE int RE_CTZ_u32(RE_u32 x)  { return x ? __builtin_ctz(x)  : 32; }
RE_INLINE int RE_POPCNT_u32(RE_u32 x){ return __builtin_popcount(x); }

RE_INLINE int RE_CLZ_U64(RE_u64 x)  { return x ? __builtin_clzll(x) : 64; }
RE_INLINE int RE_CTZ_U64(RE_u64 x)  { return x ? __builtin_ctzll(x) : 64; }
RE_INLINE int RE_POPCNT_U64(RE_u64 x){ return __builtin_popcountll(x); }
#else
/* portable fallbacks */
RE_INLINE int RE_CLZ_u32(RE_u32 x) {
    if (x == 0) return 32;
    int n = 0;
    if ((x & 0xFFFF0000u) == 0) { n += 16; x <<= 16; }
    if ((x & 0xFF000000u) == 0) { n += 8;  x <<= 8; }
    if ((x & 0xF0000000u) == 0) { n += 4;  x <<= 4; }
    if ((x & 0xC0000000u) == 0) { n += 2;  x <<= 2; }
    if ((x & 0x80000000u) == 0) { n += 1; }
    return n;
}
RE_INLINE int RE_CTZ_u32(RE_u32 x) {
    if (x == 0) return 32;
    int n = 0;
    if ((x & 0x0000FFFFu) == 0) { n += 16; x >>= 16; }
    if ((x & 0x000000FFu) == 0) { n += 8;  x >>= 8; }
    if ((x & 0x0000000Fu) == 0) { n += 4;  x >>= 4; }
    if ((x & 0x00000003u) == 0) { n += 2;  x >>= 2; }
    if ((x & 0x00000001u) == 0) { n += 1; }
    return n;
}
RE_INLINE int RE_POPCNT_u32(RE_u32 x) {
    /* Hacker's Delight */
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0F0F0F0Fu;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x3F;
}

RE_INLINE int RE_CLZ_U64(RE_u64 x) {
    if (x == 0) return 64;
    RE_u32 hi = (RE_u32)(x >> 32);
    if (hi != 0) return RE_CLZ_u32(hi);
    return 32 + RE_CLZ_u32((RE_u32)x);
}
RE_INLINE int RE_CTZ_U64(RE_u64 x) {
    if (x == 0) return 64;
    RE_u32 lo = (RE_u32)(x & 0xFFFFFFFFu);
    if (lo != 0) return RE_CTZ_u32(lo);
    return 32 + RE_CTZ_u32((RE_u32)(x >> 32));
}
RE_INLINE int RE_POPCNT_U64(RE_u64 x) {
    RE_u32 lo = (RE_u32)(x & 0xFFFFFFFFu);
    RE_u32 hi = (RE_u32)(x >> 32);
    return RE_POPCNT_u32(lo) + RE_POPCNT_u32(hi);
}
#endif

/* ---------------------------
   Branchless min/max/clamp helpers
   Works for unsigned and signed ints and for floats via bit hacks.
   --------------------------- */

/* Integer branchless min/max (32-bit signed) */
RE_INLINE RE_i32 RE_MIN_I32(RE_i32 a, RE_i32 b) {
    RE_i32 diff = a - b;
    RE_i32 mask = diff >> 31; /* -1 if diff < 0 */
    return b + (diff & mask);
}
RE_INLINE RE_i32 RE_MAX_I32(RE_i32 a, RE_i32 b) {
    RE_i32 diff = a - b;
    RE_i32 mask = diff >> 31;
    return a - (diff & mask);
}

/* Unsigned */
RE_INLINE RE_u32 RE_MIN_u32(RE_u32 a, RE_u32 b) { return (a < b) ? a : b; }
RE_INLINE RE_u32 RE_MAX_u32(RE_u32 a, RE_u32 b) { return (a > b) ? a : b; }

/* Float min/max (branchless using comparisons) */
RE_INLINE RE_f32 RE_MIN_f32(RE_f32 a, RE_f32 b) { return (a < b) ? a : b; }
RE_INLINE RE_f32 RE_MAX_f32(RE_f32 a, RE_f32 b) { return (a > b) ? a : b; }

/* Clamp variants */
RE_INLINE RE_f32 RE_CLAMP_f32(RE_f32 x, RE_f32 lo, RE_f32 hi) {
    return RE_MIN_f32(RE_MAX_f32(x, lo), hi);
}
RE_INLINE RE_i32 RE_CLAMP_I32(RE_i32 x, RE_i32 lo, RE_i32 hi) {
    return RE_MIN_I32(RE_MAX_I32(x, lo), hi);
}

/* ---------------------------
   IEEE-754 float tests using bit fields (fast)
   --------------------------- */

/* extract exponent and mantissa for f32 */
RE_INLINE void RE_DECOMPOSE_f32(RE_f32 x, int *out_sign, int *out_exp, RE_u32 *out_mant) {
    RE_f32U u; u.f = x;
    *out_sign = (u.u >> 31) & 1u;
    *out_exp  = (int)((u.u >> 23) & 0xFFu);
    *out_mant = u.u & 0x7FFFFFu;
}

/* isnan/isinf/isfinite for f32 (fast) */
RE_INLINE RE_bool RE_ISNAN_f32(RE_f32 x) {
    RE_f32U u; u.f = x;
    RE_u32 exp = (u.u >> 23) & 0xFFu;
    RE_u32 mant = u.u & 0x7FFFFFu;
    return (exp == 0xFFu) && (mant != 0u);
}
RE_INLINE RE_bool RE_ISINF_f32(RE_f32 x) {
    RE_f32U u; u.f = x;
    RE_u32 exp = (u.u >> 23) & 0xFFu;
    RE_u32 mant = u.u & 0x7FFFFFu;
    return (exp == 0xFFu) && (mant == 0u);
}
RE_INLINE RE_bool RE_ISFINITE_f32(RE_f32 x) {
    RE_f32U u; u.f = x;
    RE_u32 exp = (u.u >> 23) & 0xFFu;
    return (exp != 0xFFu);
}

/* ---------------------------
   Pack / Unpack normalized integers (useful for vertex data)
   - Pack RE_f32 in [-1,1] to signed 8/16-bit integers and back
   --------------------------- */
RE_INLINE RE_i32 RE_PACK_SNORM8(RE_f32 v) {
    /* clamp then scale */
    if (v >= 1.0f) return 127;
    if (v <= -1.0f) return -128;
    RE_f32 scaled = v * 127.0f;
    /* round to nearest */
    if (scaled >= 0.0f) return (RE_i32)(scaled + 0.5f);
    return (RE_i32)(scaled - 0.5f);
}
RE_INLINE RE_f32 RE_UNPACK_SNORM8(RE_i32 q) {
    /* q in [-128,127] -> [-1,1) */
    return (q >= 0) ? ((RE_f32)q / 127.0f) : ((RE_f32)q / 128.0f);
}

RE_INLINE RE_i32 RE_PACK_SNORM16(RE_f32 v) {
    if (v >= 1.0f) return 32767;
    if (v <= -1.0f) return -32768;
    RE_f32 scaled = v * 32767.0f;
    if (scaled >= 0.0f) return (RE_i32)(scaled + 0.5f);
    return (RE_i32)(scaled - 0.5f);
}
RE_INLINE RE_f32 RE_UNPACK_SNORM16(RE_i32 q) {
    return (q >= 0) ? ((RE_f32)q / 32767.0f) : ((RE_f32)q / 32768.0f);
}

/* ---------------------------
   Misc helpers
   --------------------------- */

/* fast absolute value for f32 using bit mask (IEEE-754) */
RE_INLINE RE_f32 RE_ABS_f32(RE_f32 x) {
    RE_f32U u; u.f = x; u.u &= 0x7FFFFFFFu; return u.f;
}

/* fast copy-sign: returns |mag| with the sign of sign_val */
RE_INLINE RE_f32 RE_COPY_SIGN_f32(RE_f32 mag, RE_f32 sign_val) {
    RE_f32U a; a.f = mag;
    RE_f32U b; b.f = sign_val;
    a.u = (a.u & 0x7FFFFFFFu) | (b.u & 0x80000000u);
    return a.f;
}

/* branchless select: returns a if cond != 0 else b (cond is 0/1) */
RE_INLINE RE_u32 RE_SELECT_u32(RE_u32 cond, RE_u32 a, RE_u32 b) {
    /* normalize cond to 0 or 1 */
    RE_u32 m = (RE_u32)(-(RE_i32)(cond != 0)); /* all 1s if cond else 0 */
    return (a & m) | (b & ~m);
}

/**
 * @brief Convert hashed uint to float in [0,1).
 */
RE_INLINE RE_f32 RE_HASH_TO_f32(RE_u32 h) {
    const RE_u32 mant_mask = (1u << 23) - 1u;
    RE_u32 m = h & mant_mask;
    RE_f32U conv;
    conv.u = (127u << 23) | m; /* float in [1,2) */
    return conv.f - 1.0f;      /* now in [0,1) */
}

/* Basic 32-bit RNG structure */
typedef struct {
    RE_u32 state;
} RE_RNG32;

/* Seed the RNG */
static inline void RE_RNG32_SEED(RE_RNG32* rng, RE_u32 seed) {
    rng->state = seed ? seed : 1u;  /* avoid zero state */
}

/* Generate next 32-bit unsigned int */
static inline RE_u32 RE_RNG32_NEXT_u32(RE_RNG32* rng) {
    /* Xorshift32 — fast, deterministic, good for graphics */
    RE_u32 x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

/**
 * @brief RNG next float in [0,1).
 */
RE_INLINE RE_f32 RE_RNG32_NEXT_f32(RE_RNG32 *rng) {
    return RE_HASH_TO_f32(RE_RNG32_NEXT_u32(rng));
}

/**
 * @brief RNG float in [lo,hi).
 */
RE_INLINE RE_f32 RE_RNG32_RANGE_f32(RE_RNG32 *rng, RE_f32 lo, RE_f32 hi) {
    RE_f32 t = RE_RNG32_NEXT_f32(rng);
    return lo + t * (hi - lo);
}


/* ---------------------------
   End of re_core.h
   --------------------------- */

#endif /* RE_CORE_H */
