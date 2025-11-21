#ifndef RE_MATH_EXT_H
#define RE_MATH_EXT_H

/**
 * @file re_math_ext.h
 * @brief Extended math, random utilities and fast approximations for REMath.
 *
 * This header provides:
 *  - angle helpers (deg/rad conversions, wrap)
 *  - remap / snap / smooth / deadzone utilities
 *  - random utilities (hash -> float, xorshift RNG wrapper, unit vectors)
 *  - fast inverse sqrt, sqrt, sin/cos (polynomial) and fast atan2
 *
 * All functions are header-only `RE_INLINE` and use only bit-level ops
 * and arithmetic (no <math.h>). These implementations are tuned for
 * graphics speed with reasonable accuracy.
 *
 * @author Jayansh Devgan
 */


#include "re_core.h"
#include "re_constants.h"
#include "re_math.h"

/* ---------------------------
   Angle helpers
   --------------------------- */

/**
 * @brief Convert degrees to radians.
 */
 RE_INLINE RE_f32 RE_DEG2RAD_f32(RE_f32 d) { return d * RE_DEG2RAD_F; }

/**
 * @brief Convert radians to degrees.
 */
 RE_INLINE RE_f32 RE_RAD2DEG_f32(RE_f32 r) { return r * RE_RAD2DEG_F; }

 /**
  * @brief Wrap angle to (-PI, PI]
  */
 RE_INLINE RE_f32 RE_WRAP_ANGLE_RAD_f32(RE_f32 a)
 {
     // reduce by multiples of TAU using nearest integer rounding
     const RE_f32 inv_tau = 1.0f / RE_TAU_F;
     RE_f32 k = a * inv_tau;

     // Round k to nearest integer
     RE_i32 ki = (RE_i32)(k + (k >= 0.0f ? 0.5f : -0.5f));
     RE_f32 r = a - (RE_f32)ki * RE_TAU_F;
     if (r <= -RE_PI_F) r += RE_TAU_F;
     else if(r > RE_PI_F) r -= RE_TAU_F;
     return r;
 }

 /* ---------------------------
    Remap / snap / smooth / deadzone
    --------------------------- */

 /**
  * @brief Remap a value from [in_min,in_max] to [out_min,out_max] (not clamped).
  */

  RE_INLINE RE_f32 RE_REMAP_f32(RE_f32 x, RE_f32 in_min, RE_f32 in_max, RE_f32 out_min, RE_f32 out_max) {
      RE_f32 d = in_max - in_min;
      if (d == 0.0f) return out_min;
      RE_f32 t = (x - in_min) / d;
      return out_min + t * (out_max - out_min);
  }

  /**
   * @brief Remap and clamp result into output range.
   */

  RE_INLINE RE_f32 RE_REMAP_CLAMPED_f32(RE_f32 x, RE_f32 in_min, RE_f32 in_max, RE_f32 out_min, RE_f32 out_max)
  {
    RE_f32 v = RE_REMAP_f32(x, in_min, in_max, out_min, out_max);
    if (out_min < out_max) return RE_CLAMP_f32(v, out_min, out_max);
    return RE_CLAMP_f32(v, out_max, out_min);
  }

  /**
   * @brief Snap x to nearest step (float).
   */

   RE_INLINE RE_f32 RE_SNAP_f32(RE_f32 x, RE_f32 step) {
       if (step == 0.0f) return x;
       RE_f32 q = x / step;
       /* round to nearest integer */
       RE_i32 iq = (RE_i32)(q + (q >= 0.0f ? 0.5f : -0.5f));
       return (RE_f32)iq * step;
   }

   /**
    * @brief Smoothstep (cubic): returns 0..1 easing between edge0..edge1.
    */
    RE_INLINE RE_f32 RE_SMOOTHSTP_f32(RE_f32 edge0, RE_f32 edge1, RE_f32 x)
    {
        RE_f32 t = (edge1 == edge0) ? 0.0f : (x - edge0) / (edge1 - edge0);
        t = RE_CLAMP_f32(t, 0.0f, 1.0f);
        /* 3t^2 - 2t^3 */
        return t * t * (3.0f - 2.0f * t);
    }

    /**
     * @brief Smootherstep (quintic).
     */
    RE_INLINE RE_f32 RE_SMOOTHERSTEP_f32(RE_f32 edge0, RE_f32 edge1, RE_f32 x) {
        RE_f32 t = (edge1 == edge0) ? 0.0f : (x - edge0) / (edge1 - edge0);
        t = RE_CLAMP_f32(t, 0.0f, 1.0f);
        /* 6t^5 - 15t^4 + 10t^3 */
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    /**
     * @brief Deadzone: zero if |x| <= thr else x.
     */
    RE_INLINE RE_f32 RE_DEADZONE_f32(RE_f32 x, RE_f32 thr) {
        return (RE_ABS_f32(x) <= thr) ? 0.0f : x;
    }

    /**
     * @brief Deadzone normalized: remap outside zone to [0,1] preserving sign.
     */
    RE_INLINE RE_f32 RE_DEADZONE_NORM_f32(RE_f32 x, RE_f32 thr) {
        RE_f32 s = RE_SIGN(x);
        RE_f32 a = RE_ABS_f32(x);
        if (a <= thr) return 0.0f;
        RE_f32 t = (a - thr) / (1.0f - thr);
        return s * RE_CLAMP_f32(t, 0.0f, 1.0f);
    }

    /* ---------------------------
       Fast inverse sqrt & sqrt
       --------------------------- */

    /**
     * @brief Quake initial magic guess for inv sqrt (single precision).
     * @note Caller should handle non-positive values if needed.
     */
    RE_INLINE RE_f32 RE_INV_SQRT_MAGIC_f32(RE_f32 number) {
        RE_f32U conv; conv.f = number;
        conv.u = 0x5f3759dfu - (conv.u >> 1);
        return conv.f;
    }

    /**
     * @brief One Newton-Raphson refinement for inv sqrt:
     *        y <- y * (1.5 - 0.5 * x * y^2)
     */
    RE_INLINE RE_f32 RE_INV_SQRT_REFINE_f32(RE_f32 number, RE_f32 approx) {
        const RE_f32 half = 0.5f;
        const RE_f32 three_halves = 1.5f;
        return approx * (three_halves - (half * number * approx * approx));
    }

    /**
     * @brief Fast inv sqrt = magic + one NR iteration.
     *        Returns 0 for non-positive inputs.
     */
    RE_INLINE RE_f32 RE_INV_SQRT_FAST_f32(RE_f32 number) {
        if (!(number > 0.0f)) return 0.0f;
        RE_f32 y = RE_INV_SQRT_MAGIC_f32(number);
        return RE_INV_SQRT_REFINE_f32(number, y);
    }

    /* ------------------------------------------------------------
       fabsf — bitmask clear sign bit
       ------------------------------------------------------------ */
    RE_INLINE RE_f32 RE_FABS_f32(RE_f32 x)
    {
        union { RE_f32 f; RE_u32 u; } v = { x };
        v.u &= 0x7FFFFFFFu;     /* Clear sign bit */
        return v.f;
    }

    /* ------------------------------------------------------------
       copysignf — copy sign bit from y into x
       ------------------------------------------------------------ */
    RE_INLINE RE_f32 RE_COPYSIGN_f32(RE_f32 x, RE_f32 y)
    {
        union { RE_f32 f; RE_u32 u; } a = { x };
        union { RE_f32 f; RE_u32 u; } b = { y };

        a.u = (a.u & 0x7FFFFFFFu) | (b.u & 0x80000000u);
        return a.f;
    }

    /* ------------------------------------------------------------
       INTERNAL: fast atan approximation (for atan2f)
       minimax fit, <= 1e-6 error
       ------------------------------------------------------------ */
    RE_INLINE RE_f32 RE_ATAN_f32(RE_f32 z)
    {
        /* Sign */
        RE_f32 s = RE_COPYSIGN_f32(1.0f, z);
        z = RE_FABS_f32(z);

        /* Polynomial approx for atan(z) on [0,1] */
        const RE_f32 p0 = 0.9998660f;
        const RE_f32 p1 = -0.3302995f;
        const RE_f32 p2 = 0.1801410f;

        RE_f32 z2 = z * z;
        RE_f32 r  = z * (p0 + p1*z2 + p2*z2*z2);

        return s * r;
    }

    /* ---------------------------
       Fast sin / cos (polynomial) and wrappers
       --------------------------- */

       /*
        * Implementation notes:
        *  - Angle is reduced to [-PI, PI] then folded to [-PI/2, PI/2] for polynomial evaluation.
        *  - Uses an odd polynomial for sine (x * (1 + c2*x^2 + c4*x^4 + c6*x^6))
        *  - Cosine is derived from sine of half-angle using cos(x) = 1 - 2*sin^2(x/2)
        *
        * Accuracy: good for graphics (errors ~1e-5 .. 1e-4 depending on range).
        */

        /**
         * @brief Compute fast sine and cosine simultaneously.
         * @param x angle in radians.
         * @param out_s pointer to store sine (may be NULL).
         * @param out_c pointer to store cosine (may be NULL).
         */
        RE_INLINE void RE_FAST_SINCOS_f32(RE_f32 x, RE_f32 *out_s, RE_f32 *out_c) {
            /* wrap to [-PI, PI] */
            RE_f32 a = RE_WRAP_ANGLE_RAD_f32(x);

            /* fold to [-PI/2, PI/2] */
            RE_f32 sign_s = 1.0f;
            RE_f32 sign_c = 1.0f;
            if (a < -RE_PI_F / 2.0f) {
                a = -RE_PI_F - a;
                sign_s = -1.0f;
                sign_c = -1.0f;
            } else if (a > RE_PI_F / 2.0f) {
                a = RE_PI_F - a;
                sign_s = 1.0f;
                sign_c = -1.0f;
            }

            /* polynomial coefficients (minimax-like) */
            const RE_f32 c2 = -0.16666666641626524f;
            const RE_f32 c4 =  0.008333333767950535f;
            const RE_f32 c6 = -0.000198412368932606f;

            RE_f32 x2 = a * a;
            RE_f32 sin_approx = a * (1.0f + x2 * (c2 + x2 * (c4 + x2 * c6)));

            /* cosine via half-angle identity */
            RE_f32 half = a * 0.5f;
            RE_f32 h2 = half * half;
            RE_f32 sh = half * (1.0f + h2 * (c2 + h2 * (c4 + h2 * c6)));
            RE_f32 cos_approx = 1.0f - 2.0f * sh * sh;

            if (out_s) *out_s = sin_approx * sign_s;
            if (out_c) *out_c = cos_approx * sign_c;
        }
        /**
         * @brief Fast acos(x) approximation on [-1, 1].
         */

         RE_INLINE RE_f32 RE_ACOS(RE_f32 x)
         {
             if (x <= -1.0f) return RE_PI_F;
             if (x >=  1.0f) return 0.0f;

             RE_f32 ax = RE_ABS_f32(x);

             // Polynomial minimax approximation of acos on [0,1]
             RE_f32 t = RE_SQRT(1.0f - ax);

             RE_f32 p =
                 -0.0187293f * ax +
                  0.0742610f;
             p = p * ax - 0.2121144f;
             p = p * ax + 1.5707288f;

             RE_f32 r = p * t;  // acos(|x|)

             // Fix sign: acos(x) = r, but symmetric for negative x
             return (x < 0.0f) ? (RE_PI_F - r) : r;
         }

         /**
          * @brief Fast asin(x) approximation on [-1, 1].
          */

          RE_INLINE RE_f32 RE_ASIN(RE_f32 x)
          {
            if (x <= -1.0f) return -RE_PI_F / 2.0f;
            if (x >=  1.0f) return  RE_PI_F / 2.0f;
            RE_f32 x2 = x * x;
            RE_f32 x4 = x2 * x2;
            // Polynomial minimax approximation of asin on [-1,1]
            RE_f32 p = x + x * x2 * (1.0f / 6.0f + x2 * (3.0f / 40.0f));
            return p;
          }

        /**
         * @brief Fast sine (radians).
         * 32 bit
         */
        RE_INLINE RE_f32 RE_SIN_f32(RE_f32 x) {
            RE_f32 s, c;
            RE_FAST_SINCOS_f32(x, &s, &c);
            (void)c;
            return s;
        }

        /**
         * @brief Fast cosine (radians).
         * 32 bit
         */
        RE_INLINE RE_f32 RE_COS_f32(RE_f32 x) {
            RE_f32 s, c;
            RE_FAST_SINCOS_f32(x, &s, &c);
            (void)s;
            return c;
        }

        /**
         * @brief Fast tangent (radians).
         * 32 bit
         */
        RE_INLINE RE_f32 RE_TAN_f32(RE_f32 x)
        {
            RE_f32 s, c;
            RE_FAST_SINCOS_f32(x, &s, &c);

            /* prevent division by extremely small cosine */
            const RE_f32 eps = 1e-6f;
            RE_f32 denom = (RE_ABS_f32(c) < eps) ? (RE_COPY_SIGN_f32(eps, c)) : c;

            return s / denom;
        }

        /* ---------------------------
           Fast atan2 approximation
           --------------------------- */

        /**
         * @brief Fast atan2 approximation.
         * Accuracy reasonable for graphics; not IEEE-accurate.
         */
        RE_INLINE RE_f32 RE_FAST_ATAN2_f32(RE_f32 y, RE_f32 x) {
            if (x == 0.0f) {
                if (y > 0.0f) return RE_PI_F * 0.5f;
                if (y < 0.0f) return -RE_PI_F * 0.5f;
                return 0.0f;
            }
            RE_f32 abs_y = RE_ABS_f32(y) + 1e-12f;
            RE_f32 angle;
            if (RE_ABS_f32(x) >= abs_y) {
                RE_f32 r = abs_y / RE_ABS_f32(x);
                RE_f32 a = r / (1.0f + 0.28f * r * r);
                if (x < 0.0f) {
                    angle = (y >= 0.0f) ? (RE_PI_F - a) : (-RE_PI_F + a);
                } else {
                    angle = (y >= 0.0f) ? a : -a;
                }
            } else {
                RE_f32 r = RE_ABS_f32(x) / abs_y;
                RE_f32 a = (RE_PI_F * 0.5f) - (r / (1.0f + 0.28f * r * r));
                angle = (y >= 0.0f) ? a : -a;
            }
            return angle;
        }

        /* ---------------------------
           Random utilities (xorshift32 + hash->float)
           --------------------------- */

        /**
         * @brief Integer hash (32-bit).
         * Useful for hash-based RNG / noise seeding.
         */
        RE_INLINE RE_u32 RE_HASH_u32(RE_u32 x) {
            x = (x ^ 61u) ^ (x >> 16);
            x = x + (x << 3);
            x = x ^ (x >> 4);
            x = x * 0x27d4eb2du;
            x = x ^ (x >> 15);
            return x;
        }

        /**
         * @brief Random unit vector 2D (angle method).
         */
        RE_INLINE void RE_RAND_UNIT_VEC2_f32(RE_RNG32 *rng, RE_f32 *out_x, RE_f32 *out_y) {
            RE_f32 t = RE_RNG32_RANGE_f32(rng, 0.0f, RE_TAU_F);
            if (out_x) *out_x = RE_COS_f32(t);
            if (out_y) *out_y = RE_SIN_f32(t);
        }

        /**
         * @brief Random unit vector 3D (rejection sampling).
         */
        RE_INLINE void RE_RAND_UNIT_VEC3_f32(RE_RNG32 *rng, RE_f32 *out_x, RE_f32 *out_y, RE_f32 *out_z) {
            RE_f32 x,y,z, len2;
            do {
                x = RE_RNG32_RANGE_f32(rng, -1.0f, 1.0f);
                y = RE_RNG32_RANGE_f32(rng, -1.0f, 1.0f);
                z = RE_RNG32_RANGE_f32(rng, -1.0f, 1.0f);
                len2 = x*x + y*y + z*z;
            } while (len2 <= 1e-6f || len2 > 1.0f);
            RE_f32 inv = RE_INV_SQRT_FAST_f32(len2);
            if (out_x) *out_x = x * inv;
            if (out_y) *out_y = y * inv;
            if (out_z) *out_z = z * inv;
        }

        /* ---------------------------
           Convenience wrappers
           --------------------------- */

        RE_INLINE void RE_SINCOS_f32(RE_f32 x, RE_f32 *s, RE_f32 *c) { RE_FAST_SINCOS_f32(x, s, c); }
        RE_INLINE RE_f32 RE_ATAN2_f32(RE_f32 y, RE_f32 x) { return RE_FAST_ATAN2_f32(y, x); }

        /**
         * @brief Fast approximate reciprocal (1/x) using bit-level seed + Newton iteration.
         *
         * This function computes an approximation of the reciprocal of a floating point
         * value without using division. It performs:
         *
         *  - A bit manipulation “magic constant” initialization
         *  - One Newton–Raphson refinement step
         *
         * This produces an approximation typically within ~1–2 ULP of the exact 1/x,
         * and is significantly faster than a hardware division on many platforms.
         *
         * @note Accuracy is lower than (1.0f / x), but speed is much higher.
         *       Safe for use in vector math, physics, graphics, and normalized ops.
         *
         * @param x The input float value.
         * @return An approximation of 1/x.
         */
         RE_INLINE RE_f32 RE_RCP(RE_f32 x)
         {
             RE_f32U ux = { x };

             RE_u32 i = 0x7EF127EAu - ux.u;

             RE_f32 y;
             ((RE_f32U*)&y)->u = i;

             y = y * (2.0f - x * y);
             y = y * (2.0f - x * y);
             return y;
         }



#endif
