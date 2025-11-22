#ifndef RE_RANDOM_H
#define RE_RANDOM_H

#include "re_core.h"
#include "re_math.h"
#include "re_quat.h"

/* ============================================================================
    RANDOM STATE (PCG32)
   ========================================================================== */

typedef struct {
    RE_u64 state;
    RE_u64 inc;
} RE_RANDOM_STATE;

/* ============================================================================
    PCG32 CORE
    return 32-bit random number
   ========================================================================== */
RE_INLINE RE_u32 RE_RANDOM_U32(RE_RANDOM_STATE* rng)
{
    RE_u64 old = rng->state;

    /* Advance LCG state */
    rng->state = old * 6364136223846793005ULL + (rng->inc | 1ULL);

    /* Output transform */
    RE_u32 xorshift = (RE_u32)(((old >> 18u) ^ old) >> 27u);
    RE_u32 rot = (RE_u32)(old >> 59u);

    return (xorshift >> rot) | (xorshift << ((-rot) & 31));
}

/* ============================================================================
    FLOAT RANDOM (0..1)
   ========================================================================== */

RE_INLINE RE_f32 RE_RANDOM_F32(RE_RANDOM_STATE* rng)
{
    /* Divide by 2^32 */
    return (RE_f32)(RE_RANDOM_U32(rng)) * (1.0f / 4294967296.0f);
}

RE_INLINE RE_f64 RE_RANDOM_F64(RE_RANDOM_STATE* rng)
{
    return (RE_f64)(RE_RANDOM_U32(rng)) * (1.0 / 4294967296.0);
}

/* ============================================================================
    RANGED RANDOM
   ========================================================================== */

RE_INLINE RE_u32 RE_RANDOM_RANGE_U32(RE_RANDOM_STATE* rng, RE_u32 min, RE_u32 max)
{
    return min + (RE_RANDOM_U32(rng) % (max - min + 1));
}

RE_INLINE RE_f32 RE_RANDOM_RANGE_F32(RE_RANDOM_STATE* rng, RE_f32 min, RE_f32 max)
{
    return min + (max - min) * RE_RANDOM_F32(rng);
}

RE_INLINE RE_f64 RE_RANDOM_RANGE_F64(RE_RANDOM_STATE* rng, RE_f64 min, RE_f64 max)
{
    return min + (max - min) * RE_RANDOM_F64(rng);
}

/* ============================================================================
    RANDOM 2D / 3D UNIT VECTORS
   ========================================================================== */

RE_INLINE RE_V2_f32 RE_RANDOM_UNIT2_F32(RE_RANDOM_STATE* rng)
{
    RE_f32 a = RE_RANDOM_RANGE_F32(rng, 0.0f, RE_TAU_F);
    RE_V2_f32 v = { RE_COS_f32(a), RE_SIN_f32(a) };
    return v;
}

RE_INLINE RE_V3_f32 RE_RANDOM_UNIT3_F32(RE_RANDOM_STATE* rng)
{
    /* Spherical distribution */
    RE_f32 z   = RE_RANDOM_RANGE_F32(rng, -1.0f, 1.0f);
    RE_f32 a   = RE_RANDOM_RANGE_F32(rng, 0.0f, RE_TAU_F);

    RE_f32 r   = RE_SQRT(1.0f - z*z);
    RE_V3_f32 v = { r*RE_COS_f32(a), r*RE_SIN_f32(a), z };
    return v;
}

/* ============================================================================
    RANDOM QUATERNION (uniformly distributed)
   ========================================================================== */

RE_INLINE RE_QUAT_f32 RE_RANDOM_QUAT_F32(RE_RANDOM_STATE* rng)
{
    RE_f32 u1 = RE_RANDOM_F32(rng);
    RE_f32 u2 = RE_RANDOM_F32(rng);
    RE_f32 u3 = RE_RANDOM_F32(rng);

    RE_f32 s1 = RE_SQRT(1.0f - u1);
    RE_f32 s2 = RE_SQRT(u1);

    RE_f32 t1 = RE_TAU_F * u2;
    RE_f32 t2 = RE_TAU_F * u3;

    RE_QUAT_f32 q =
    {
        RE_SIN_f32(t1) * s1,
        RE_COS_f32(t1) * s1,
        RE_SIN_f32(t2) * s2,
        RE_COS_f32(t2) * s2
    };
    return q;
}

/* Seed generator */
RE_INLINE RE_RANDOM_STATE RE_RANDOM_SEED(RE_u64 seed, RE_u64 seq)
{
    RE_RANDOM_STATE s;
    s.state = 0;
    s.inc   = (seq << 1u) | 1u;

    /* Warm-up */
    (void)RE_RANDOM_U32(&s);
    s.state += seed;
    (void)RE_RANDOM_U32(&s);

    return s;
}


#endif /* RE_RANDOM_H */
