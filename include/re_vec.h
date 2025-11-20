#ifndef RE_VEC_H
#define RE_VEC_H

/**
 * @file re_vec.h
 * @brief Strict typed V2 — fully generated for all numeric types.
 *
 * This header generates:
 *  - typed V2 structs for all numeric scalar types used in REMath
 *  - per-type functions (no implicit/automatic casting)
 *
 * Naming convention:
 *   typedef:  RE_V2_<sfx>         (e.g. RE_V2_f32, RE_V2_i32)
 *   constructor: RE_V2_<sfx>_make(...)
 *   broadcast:   RE_V2_<sfx>_broadcast(...)
 *   ops:         RE_V2_<sfx>_add / _sub / _scale / _div / _dot / _hadamard / _clamp / _lerp
 *
 * Author: Jayansh Devgan
 */

 #include "re_core.h"
 #include "re_math_ext.h"

 /* ---------------------------------------------------------------------------
    Types available (expected to be defined in re_core.h):
      RE_f32, RE_f64,
      RE_i8, RE_i16, RE_i32, RE_i64,
      RE_u8, RE_u16, RE_u32, RE_u64
    --------------------------------------------------------------------------- */

 /* ======================================================================================
    Macro that generates a typed V2 struct and its functions (narrowing-safe casts)
    ====================================================================================== */

    /**
     * @brief Generate typed V2 type + functions for scalar type `T` and suffix `SFX`.
     *
     * Produces:
     *   typedef struct { T x,y; } RE_V2_SFX;
     *   RE_V2_SFX RE_V2_SFX_make(T x, T y);
     *   RE_V2_SFX RE_V2_SFX_zero(void);
     *   RE_V2_SFX RE_V2_SFX_broadcast(T a);
     *   RE_V2_SFX RE_V2_SFX_add(RE_V2_SFX a, RE_V2_SFX b);
     *   RE_V2_SFX RE_V2_SFX_sub(RE_V2_SFX a, RE_V2_SFX b);
     *   RE_V2_SFX RE_V2_SFX_scale(RE_V2_SFX v, T s);
     *   RE_V2_SFX RE_V2_SFX_div(RE_V2_SFX v, T s);
     *   T          RE_V2_SFX_dot(RE_V2_SFX a, RE_V2_SFX b);
     *   RE_V2_SFX RE_V2_SFX_hadamard(RE_V2_SFX a, RE_V2_SFX b);
     *   RE_V2_SFX RE_V2_SFX_clamp(RE_V2_SFX v, RE_V2_SFX mn, RE_V2_SFX mx);
     *   RE_V2_SFX RE_V2_SFX_lerp(RE_V2_SFX a, RE_V2_SFX b, T t);
     *
     * All returned struct initializers explicitly cast to (T) to avoid narrowing diagnostics.
     */
     #define RE_GEN_V2_TYPE_AND_FUNCS(T, SFX)                                            \
                                                                                        \
         typedef struct { T x, y; } RE_V2_##SFX;                                        \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_MAKE_##SFX(T _x, T _y) {                          \
             return (RE_V2_##SFX){ (T)(_x), (T)(_y) };                                  \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_ZERO_##SFX(void) {                               \
             return (RE_V2_##SFX){ (T)0, (T)0 };                                        \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_BROADCAST_##SFX(T a) {                           \
             return (RE_V2_##SFX){ (T)a, (T)a };                                        \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_ADD_##SFX(RE_V2_##SFX a, RE_V2_##SFX b) {        \
             return (RE_V2_##SFX){ (T)(a.x + b.x), (T)(a.y + b.y) };                    \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_SUB_##SFX(RE_V2_##SFX a, RE_V2_##SFX b) {        \
             return (RE_V2_##SFX){ (T)(a.x - b.x), (T)(a.y - b.y) };                    \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_SCALE_##SFX(RE_V2_##SFX v, T s) {                \
             return (RE_V2_##SFX){ (T)(v.x * s), (T)(v.y * s) };                        \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_DIV_##SFX(RE_V2_##SFX v, T s) {                  \
             if ((s) == (T)0) {                                                         \
                 /* define behavior: return zero-vector when dividing by zero */       \
                 return (RE_V2_##SFX){ (T)0, (T)0 };                                    \
             }                                                                          \
             /* perform division in higher precision then cast back to T */            \
             /* use RE_f64 for integer and float safety */                              \
             RE_f64 inv = 1.0 / (RE_f64)(s);                                            \
             return (RE_V2_##SFX){ (T)((RE_f64)v.x * inv), (T)((RE_f64)v.y * inv) };    \
         }                                                                              \
                                                                                        \
         RE_INLINE T RE_DOT_V2_##SFX(RE_V2_##SFX a, RE_V2_##SFX b) {                  \
             /* return same scalar domain T (user can promote if needed) */            \
             /* compute in RE_f64 then cast to T to reduce intermediate overflow */     \
             RE_f64 t = (RE_f64)a.x * (RE_f64)b.x + (RE_f64)a.y * (RE_f64)b.y;          \
             return (T)t;                                                               \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_HADAMARD_##SFX(RE_V2_##SFX a, RE_V2_##SFX b) {   \
             return (RE_V2_##SFX){ (T)(a.x * b.x), (T)(a.y * b.y) };                    \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_CLAMP_##SFX(RE_V2_##SFX v,                        \
                                                   RE_V2_##SFX mn, RE_V2_##SFX mx) {    \
             return (RE_V2_##SFX){                                                     \
                 (T)RE_CLAMP(v.x, mn.x, mx.x),                                          \
                 (T)RE_CLAMP(v.y, mn.y, mx.y)                                           \
             };                                                                         \
         }                                                                              \
                                                                                        \
         RE_INLINE RE_V2_##SFX RE_V2_LERP_##SFX(RE_V2_##SFX a, RE_V2_##SFX b, T t) {  \
             /* compute in RE_f64 for precision, then cast to T */                     \
             RE_f64 xa = (RE_f64)a.x;                                                   \
             RE_f64 xb = (RE_f64)b.x;                                                   \
             RE_f64 ya = (RE_f64)a.y;                                                   \
             RE_f64 yb = (RE_f64)b.y;                                                   \
             RE_f64 tf = (RE_f64)t;                                                     \
             RE_f64 rx = xa + (xb - xa) * tf;                                           \
             RE_f64 ry = ya + (yb - ya) * tf;                                           \
             return (RE_V2_##SFX){ (T)rx, (T)ry };                                      \
         }
    /* ======================================================================================
    Instantiate typed V2 for all requested scalar types
    ====================================================================================== */

    /* floating */
    RE_GEN_V2_TYPE_AND_FUNCS(RE_f32, f32)
    RE_GEN_V2_TYPE_AND_FUNCS(RE_f64, f64)

    /* signed integers */
    RE_GEN_V2_TYPE_AND_FUNCS(RE_i8,  i8)
    RE_GEN_V2_TYPE_AND_FUNCS(RE_i16, i16)
    RE_GEN_V2_TYPE_AND_FUNCS(RE_i32, i32)
    RE_GEN_V2_TYPE_AND_FUNCS(RE_i64, i64)

    /* unsigned integers */
    RE_GEN_V2_TYPE_AND_FUNCS(RE_u8,  u8)
    RE_GEN_V2_TYPE_AND_FUNCS(RE_u16, u16)
    RE_GEN_V2_TYPE_AND_FUNCS(RE_u32, u32)
    RE_GEN_V2_TYPE_AND_FUNCS(RE_u64, u64)

    /* ============================================================================================
       V2 (float-only) advanced math – RE_V2_f32 and RE_V2_f64
       ============================================================================================ */

    /**
     * @name V2 advanced operations (float-only)
     * @{
     */

    /* ============================================================
       LENGTH (precise)
       ============================================================ */

    /**
     * @brief Compute precise vector length (L2 norm).
     * @param v Input vector.
     * @return Length as RE_f32.
     */
    // RE_INLINE RE_f32 RE_LENGTH_V2_f32(RE_V2_f32 v) {
    //     RE_f32 dot = (RE_f32)((RE_f64)v.x * (RE_f64)v.x + (RE_f64)v.y * (RE_f64)v.y); // outdated implimentation
    //     return RE_SQRT(dot);
    // }

    /**
     * @brief Compute precise vector length (L2 norm).
     * @param v Input vector.
     * @return Length as RE_f64.
     */
    // RE_INLINE RE_f64 RE_LENGTH_V2_f64(RE_V2_f64 v) {
    //     RE_f64 dot = (v.x * v.x) + (v.y * v.y);  // outdated implimentation
    //     return RE_SQRT(dot);
    // }

    /* ============================================================
       LENGTH (fast)
       ============================================================ */

    /**
     * @brief Fast approximate length (uses fast rsqrt).
     * @param v Input vector.
     * @return Approximate length (RE_f32).
     */
    RE_INLINE RE_f32 RE_V2_LENGTH_f32(RE_V2_f32 v) {
        RE_f32 dot = v.x * v.x + v.y * v.y;
        return dot * RE_INVSQRT(dot);   // |v| ≈ dot * (1/sqrt(dot))
    }

    /**
     * @brief Fast approximate length (RE_f64 implementation).
     * @warning f64 fast version is less accurate; uses float seed + NR.
     */
    RE_INLINE RE_f64 RE_V2_LENGTH_f64(RE_V2_f64 v) {
        RE_f64 dot = (v.x * v.x) + (v.y * v.y);

        /* float seed for rsqrt */
        RE_f32 seed_f = RE_INVSQRT((RE_f32)dot);
        RE_f64 y = (RE_f64)seed_f;

        /* One step Newton refine for double precision */
        y = y * (1.5 - 0.5 * dot * y * y);

        return dot * y;
    }

    /* ============================================================
       NORMALIZE (precise)
       ============================================================ */

    /**
     * @brief Normalize vector to unit length (precise division).
     * @param v Input vector.
     * @return Normalized vector. Returns zero-vector if |v| = 0.
     */
    // RE_INLINE RE_V2_f32 RE_V2_f32_normalize(RE_V2_f32 v) { // outdated implementation
    //     RE_f32 len = RE_V2_f32_length(v);
    //     if (len <= 0.0f) return RE_V2_f32_zero();
    //     RE_f32 inv = RE_RCP(len);
    //     return RE_V2_f32_make(v.x * inv, v.y * inv);
    // }

    /**
     * @brief Normalize vector to unit length (precise).
     * @param v Input vector.
     * @return Normalized vector. Returns zero-vector if |v| = 0.
     */
    // RE_INLINE RE_V2_f64 RE_V2_f64_normalize(RE_V2_f64 v) { // outdated implementation
    //     RE_f64 len = RE_V2_f64_length(v);
    //     if (len <= 0.0) return RE_V2_f64_zero();
    //     RE_f64 inv = RE_RCP(len);
    //     return RE_V2_f64_make((RE_f64)v.x * inv, (RE_f64)v.y * inv);
    // }

    /* ============================================================
       NORMALIZE (fast)
       ============================================================ */

    /**
     * @brief Fast approximate normalize using rsqrt.
     * @param v Input vector.
     * @return Approx normalized unit vector.
     */
    RE_INLINE RE_V2_f32 RE_V2_NORMALIZE_f32(RE_V2_f32 v) {
        RE_f32 dot = v.x*v.x + v.y*v.y;

        if (dot <= 0.0f) return RE_V2_ZERO_f32();

        RE_f32 inv = RE_INVSQRT(dot);
        return RE_V2_MAKE_f32(v.x * inv, v.y * inv);
    }

    /**
     * @brief Fast approximate normalize for f64.
     */
    RE_INLINE RE_V2_f64 RE_V2_NORMALIZE_f64(RE_V2_f64 v) {
        RE_f64 dot = (v.x*v.x + v.y*v.y);
        if (dot <= 0.0) return RE_V2_ZERO_f64();

        /* float rsqrt seed */
        RE_f32 seed_f = RE_INVSQRT((RE_f32)dot);
        RE_f64 inv = (RE_f64)seed_f;

        /* refine */
        inv = inv * (1.5 - 0.5 * dot * inv * inv);

        return RE_V2_MAKE_f64(v.x * inv, v.y * inv);
    }

    /* ============================================================
       DISTANCE
       ============================================================ */

    /**
     * @brief Compute precise distance between two V2_f32.
     */
    RE_INLINE RE_f32 RE_V2_DISTANCE_f32(RE_V2_f32 a, RE_V2_f32 b) {
        return RE_V2_LENGTH_f32(RE_V2_SUB_f32(a, b));
    }

    /**
     * @brief Compute precise distance between two V2_f64.
     */
    RE_INLINE RE_f64 RE_V2_DISTANCE_f64(RE_V2_f64 a, RE_V2_f64 b) {
        return RE_V2_LENGTH_f64(RE_V2_SUB_f64(a, b));
    }

    /**
     * @}
     */

     /// Reflect a vector `I` around a normal `N`.
     /// Formula: R = I - 2 * dot(I, N) * N
     RE_INLINE RE_V2_f32
     RE_REFLECT_V2_f32(RE_V2_f32 I, RE_V2_f32 N)
     {
         RE_f32 d = I.x * N.x + I.y * N.y;
         return (RE_V2_f32){
             I.x - 2 * (d * N.x),
             I.y - 2 * (d * N.y)
         };
     }

     /// Refract vector `I` through normal `N` with index ratio `eta`
     /// Returns zero vector if total internal reflection occurs.
     /// Formula: T = eta * I - (eta * dot(I, N) + sqrt(k)) * N
     /// where k = 1 - eta^2 * (1 - dot(I,N)^2)
     RE_INLINE RE_V2_f32
     RE_REFRACT_V2_f32(RE_V2_f32 I, RE_V2_f32 N, RE_f32 eta)
     {
         RE_f32 d = I.x * N.x + I.y * N.y;
         RE_f32 k = 1.0f - eta * eta * (1.0f - d * d);

         if (k < 0.0f) {
             // Total internal reflection = return zero vector
             return (RE_V2_f32){0.0f, 0.0f};
         }

         RE_f32 t = eta * d + RE_SQRT(k);

         return (RE_V2_f32){
             eta * I.x - t * N.x,
             eta * I.y - t * N.y
         };
     }

     /// Returns angle between vectors A and B in radians.
     /// Formula: acos(dot(A,B) / (|A| * |B|))
     RE_INLINE RE_f32
     RE_ANGLE_V2_f32(RE_V2_f32 A, RE_V2_f32 B)
     {
         RE_f32 dot = A.x * B.x + A.y * B.y;
         RE_f32 lenA = RE_SQRT(A.x*A.x + A.y*A.y);
         RE_f32 lenB = RE_SQRT(B.x*B.x + B.y*B.y);

         RE_f32 denom = lenA * lenB;
         if (denom == 0.0f)
             return 0.0f;

         // Clamp to avoid NaN due to FP precision
         RE_f32 c = dot / denom;
         if (c < -1.0f) c = -1.0f;
         if (c >  1.0f) c =  1.0f;

         return RE_ACOS(c);
     }

     /// Projects vector A onto vector B.
     /// Formula: projB(A) = (dot(A,B) / dot(B,B)) * B
     RE_INLINE RE_V2_f32
     RE_PROJECT_V2_f32(RE_V2_f32 A, RE_V2_f32 B)
     {
         RE_f32 denom = B.x*B.x + B.y*B.y;
         if (denom == 0.0f)
             return (RE_V2_f32){0.0f, 0.0f};

         RE_f32 scale = (A.x*B.x + A.y*B.y) / denom;

         return (RE_V2_f32){
             B.x * scale,
             B.y * scale
         };
     }

     /* ===================
      *  VEC 3
      * ===================
      */

      #define RE_GEN_V3_TYPE_AND_FUNCS(T, SFX)                                                         \
                                                                                                       \
          /* ---------------------------------------------------------------------- */                 \
          /* TYPE                                                                    */                 \
          /* ---------------------------------------------------------------------- */                 \
          typedef struct { T x, y, z; } RE_V3_##SFX;                                                   \
                                                                                                       \
          /* ---------------------------------------------------------------------- */                 \
          /* BASIC CONSTRUCTORS                                                     */                 \
          /* ---------------------------------------------------------------------- */                 \
          RE_INLINE RE_V3_##SFX RE_V3_MAKE_##SFX(T x, T y, T z) {                                    \
              return (RE_V3_##SFX){ (T)x, (T)y, (T)z };                                                \
          }                                                                                            \
                                                                                                       \
          RE_INLINE RE_V3_##SFX RE_V3_ZERO_##SFX(void) {                                             \
              return (RE_V3_##SFX){ (T)0, (T)0, (T)0 };                                                \
          }                                                                                            \
                                                                                                       \
          RE_INLINE RE_V3_##SFX RE_V3_BROADCAST_##SFX(T a) {                                         \
              return (RE_V3_##SFX){ (T)a, (T)a, (T)a };                                                \
          }                                                                                            \
                                                                                                       \
          /* ---------------------------------------------------------------------- */                 \
          /* BASIC OPERATIONS                                                       */                 \
          /* ---------------------------------------------------------------------- */                 \
          RE_INLINE RE_V3_##SFX RE_V3_ADD_##SFX(RE_V3_##SFX a, RE_V3_##SFX b) {                      \
              return (RE_V3_##SFX){ (T)(a.x + b.x), (T)(a.y + b.y), (T)(a.z + b.z) };                  \
          }                                                                                            \
                                                                                                       \
          RE_INLINE RE_V3_##SFX RE_V3_SUB_##SFX(RE_V3_##SFX a, RE_V3_##SFX b) {                      \
              return (RE_V3_##SFX){ (T)(a.x - b.x), (T)(a.y - b.y), (T)(a.z - b.z) };                  \
          }                                                                                            \
                                                                                                       \
          RE_INLINE RE_V3_##SFX RE_V3_SCALE_##SFX(RE_V3_##SFX v, T s) {                              \
              return (RE_V3_##SFX){ (T)(v.x * s), (T)(v.y * s), (T)(v.z * s) };                        \
          }                                                                                            \
                                                                                                       \
          RE_INLINE RE_V3_##SFX RE_V3_DIV_##SFX(RE_V3_##SFX v, T s) {                                \
              if (s == (T)0) return RE_V3_ZERO_##SFX();                                              \
              RE_f64 inv = 1.0 / (RE_f64)s;                                                            \
              return (RE_V3_##SFX){                                                                    \
                  (T)((RE_f64)v.x * inv),                                                              \
                  (T)((RE_f64)v.y * inv),                                                              \
                  (T)((RE_f64)v.z * inv)                                                               \
              };                                                                                       \
          }                                                                                            \
                                                                                                       \
          RE_INLINE T RE_V3_DOT_##SFX(RE_V3_##SFX a, RE_V3_##SFX b) {                                \
              RE_f64 t = (RE_f64)a.x * b.x + (RE_f64)a.y * b.y + (RE_f64)a.z * b.z;                    \
              return (T)t;                                                                             \
          }                                                                                            \
                                                                                                       \
          RE_INLINE RE_V3_##SFX RE_V3_HADAMARD_##SFX(RE_V3_##SFX a, RE_V3_##SFX b) {                 \
              return (RE_V3_##SFX){ (T)(a.x*b.x), (T)(a.y*b.y), (T)(a.z*b.z) };                        \
          }                                                                                            \
                                                                                                       \
          RE_INLINE RE_V3_##SFX RE_V3_CLAMP_##SFX(RE_V3_##SFX v,                                     \
                                                   RE_V3_##SFX mn, RE_V3_##SFX mx) {                   \
              return (RE_V3_##SFX){                                                                    \
                  (T)RE_CLAMP(v.x, mn.x, mx.x),                                                        \
                  (T)RE_CLAMP(v.y, mn.y, mx.y),                                                        \
                  (T)RE_CLAMP(v.z, mn.z, mx.z)                                                         \
              };                                                                                       \
          }                                                                                            \
                                                                                                       \
          RE_INLINE RE_V3_##SFX RE_V3_LERP_##SFX(RE_V3_##SFX a, RE_V3_##SFX b, T t) {                \
              RE_f64 tf = (RE_f64)t;                                                                    \
              return (RE_V3_##SFX){                                                                     \
                  (T)(a.x + (b.x - a.x) * tf),                                                          \
                  (T)(a.y + (b.y - a.y) * tf),                                                          \
                  (T)(a.z + (b.z - a.z) * tf)                                                           \
              };                                                                                        \
          }

          /* floating */
          RE_GEN_V3_TYPE_AND_FUNCS(RE_f32, f32)
          RE_GEN_V3_TYPE_AND_FUNCS(RE_f64, f64)

          /* signed integers */
          RE_GEN_V3_TYPE_AND_FUNCS(RE_i8,  i8)
          RE_GEN_V3_TYPE_AND_FUNCS(RE_i16, i16)
          RE_GEN_V3_TYPE_AND_FUNCS(RE_i32, i32)
          RE_GEN_V3_TYPE_AND_FUNCS(RE_i64, i64)

          /* unsigned integers */
          RE_GEN_V3_TYPE_AND_FUNCS(RE_u8,  u8)
          RE_GEN_V3_TYPE_AND_FUNCS(RE_u16, u16)
          RE_GEN_V3_TYPE_AND_FUNCS(RE_u32, u32)
          RE_GEN_V3_TYPE_AND_FUNCS(RE_u64, u64)

          /* ============================================================================================
             Float-only advanced math for Vec3 (f32 / f64)
             ============================================================================================ */

          //
          // LENGTH
          //
          // RE_INLINE RE_f32 RE_V3_LENGTH_f32(RE_V3_f32 v) {
          //     RE_f32 d = v.x*v.x + v.y*v.y + v.z*v.z; // outdated implementation
          //     return RE_SQRT(d);
          // }

          // RE_INLINE RE_f64 RE_V3_LENGTH_f64(RE_V3_f64 v) {
          //     RE_f64 d = v.x*v.x + v.y*v.y + v.z*v.z; // outdated implementation
          //     return RE_SQRT(d);
          // }

          //
          // FAST LENGTH
          //
          RE_INLINE RE_f32 RE_V3_LENGTH_f32(RE_V3_f32 v) {
              RE_f32 d = v.x*v.x + v.y*v.y + v.z*v.z;
              return d * RE_INVSQRT(d);
          }

          RE_INLINE RE_f64 RE_V3_LENGTH_f64(RE_V3_f64 v) {
              RE_f64 d = v.x*v.x + v.y*v.y + v.z*v.z;
              RE_f32 seed = RE_INVSQRT((RE_f32)d);

              RE_f64 y = (RE_f64)seed;
              y = y * (1.5 - 0.5 * d * y * y); // refine

              return d * y;
          }

          //
          // NORMALIZE
          //
          RE_INLINE RE_V3_f32 RE_V3_NORMALIZE_f32(RE_V3_f32 v) {
              RE_f32 d = v.x*v.x + v.y*v.y + v.z*v.z;
              if (d <= 0.0f) return RE_V3_ZERO_f32();
              RE_f32 inv = RE_INVSQRT(d);
              return RE_V3_MAKE_f32(v.x*inv, v.y*inv, v.z*inv);
          }

          RE_INLINE RE_V3_f64 RE_V3_NORMALIZE_f64(RE_V3_f64 v) {
              RE_f64 d = v.x*v.x + v.y*v.y + v.z*v.z;
              if (d <= 0.0f) return RE_V3_ZERO_f64();

              RE_f32 seed = RE_INVSQRT((RE_f32)d);
              RE_f64 inv = (RE_f64)seed;
              inv = inv * (1.5 - 0.5 * d * inv * inv);

              return RE_V3_MAKE_f64(v.x*inv, v.y*inv, v.z*inv);
          }

          //
          // DISTANCE
          //
          RE_INLINE RE_f32 RE_V3_DISTANCE_f32(RE_V3_f32 a, RE_V3_f32 b) {
              return RE_V3_LENGTH_f32(RE_V3_SUB_f32(a, b));
          }

          RE_INLINE RE_f64 RE_V3_DISTANCE_f64(RE_V3_f64 a, RE_V3_f64 b) {
              return RE_V3_LENGTH_f64(RE_V3_SUB_f64(a, b));
          }

          //
          // CROSS PRODUCT (float only)
          //
          RE_INLINE RE_V3_f32 RE_V3_CROSS_f32(RE_V3_f32 a, RE_V3_f32 b) {
              return RE_V3_MAKE_f32(
                  a.y*b.z - a.z*b.y,
                  a.z*b.x - a.x*b.z,
                  a.x*b.y - a.y*b.x
              );
          }

          RE_INLINE RE_V3_f64 RE_V3_CROSS_f64(RE_V3_f64 a, RE_V3_f64 b) {
              return RE_V3_MAKE_f64(
                  a.y*b.z - a.z*b.y,
                  a.z*b.x - a.x*b.z,
                  a.x*b.y - a.y*b.x
              );
          }

          //
          // REFLECT
          //
          RE_INLINE RE_V3_f32 RE_V3_REFLECT_f32(RE_V3_f32 I, RE_V3_f32 N) {
              RE_f32 d = RE_V3_DOT_f32(I, N);
              return RE_V3_MAKE_f32(
                  I.x - 2 * (d*N.x),
                  I.y - 2 * (d*N.y),
                  I.z - 2 * (d*N.z)
              );
          }

          //
          // REFRACT
          //
          RE_INLINE RE_V3_f32 RE_V3_REFRACT_f32(RE_V3_f32 I, RE_V3_f32 N, RE_f32 eta) {
              RE_f32 d = RE_V3_DOT_f32(I, N);
              RE_f32 k = 1.0f - eta*eta*(1.0f - d*d);

              if (k < 0.0f) return RE_V3_ZERO_f32();

              RE_f32 t = eta*d + RE_SQRT(k);
              return RE_V3_MAKE_f32(
                  eta*I.x - t*N.x,
                  eta*I.y - t*N.y,
                  eta*I.z - t*N.z
              );
          }

          //
          // ANGLE
          //
          RE_INLINE RE_f32 RE_V3_ANGLE_f32(RE_V3_f32 A, RE_V3_f32 B) {
              RE_f32 dot = RE_V3_DOT_f32(A, B);
              RE_f32 lenA = RE_V3_LENGTH_f32(A);
              RE_f32 lenB = RE_V3_LENGTH_f32(B);

              RE_f32 d = lenA * lenB;
              if (d <= 0.0f) return 0.0f;

              RE_f32 c = dot / d;
              c = RE_CLAMP(c, -1.0f, 1.0f);

              return RE_ACOS(c);
          }

          //
          // PROJECT
          //
          RE_INLINE RE_V3_f32 RE_V3_PROJECT_f32(RE_V3_f32 A, RE_V3_f32 B) {
              RE_f32 denom = RE_V3_DOT_f32(B, B);
              if (denom == 0.0f) return RE_V3_ZERO_f32();

              RE_f32 scale = RE_V3_DOT_f32(A, B) / denom;
              return RE_V3_MAKE_f32(B.x*scale, B.y*scale, B.z*scale);
          }

          /* ===================
           *  VEC 4
           * ===================
           */

           #define RE_GEN_V4_TYPE_AND_FUNCS(T, SFX)                                                         \
                                                                                                            \
               /* ---------------------------------------------------------------------- */                 \
               /* TYPE                                                                    */                 \
               /* ---------------------------------------------------------------------- */                 \
               typedef struct { T x, y, z, w; } RE_V4_##SFX;                                                \
                                                                                                            \
               /* ---------------------------------------------------------------------- */                 \
               /* BASIC CONSTRUCTORS                                                     */                 \
               /* ---------------------------------------------------------------------- */                 \
               RE_INLINE RE_V4_##SFX RE_V4_MAKE_##SFX(T x, T y, T z, T w) {                               \
                   return (RE_V4_##SFX){ (T)x, (T)y, (T)z, (T)w };                                          \
               }                                                                                            \
                                                                                                            \
               RE_INLINE RE_V4_##SFX RE_V4_ZERO_##SFX(void) {                                             \
                   return (RE_V4_##SFX){ (T)0, (T)0, (T)0, (T)0 };                                          \
               }                                                                                            \
                                                                                                            \
               RE_INLINE RE_V4_##SFX RE_V4_BROADCAST_##SFX(T a) {                                         \
                   return (RE_V4_##SFX){ (T)a, (T)a, (T)a, (T)a };                                          \
               }                                                                                            \
                                                                                                            \
               /* ---------------------------------------------------------------------- */                 \
               /* BASIC OPS                                                              */                 \
               /* ---------------------------------------------------------------------- */                 \
               RE_INLINE RE_V4_##SFX RE_V4_ADD_##SFX(RE_V4_##SFX a, RE_V4_##SFX b) {                      \
                   return (RE_V4_##SFX){                                                                    \
                       (T)(a.x + b.x), (T)(a.y + b.y),                                                     \
                       (T)(a.z + b.z), (T)(a.w + b.w)                                                      \
                   };                                                                                       \
               }                                                                                            \
                                                                                                            \
               RE_INLINE RE_V4_##SFX RE_V4_SUB_##SFX(RE_V4_##SFX a, RE_V4_##SFX b) {                      \
                   return (RE_V4_##SFX){                                                                    \
                       (T)(a.x - b.x), (T)(a.y - b.y),                                                     \
                       (T)(a.z - b.z), (T)(a.w - b.w)                                                      \
                   };                                                                                       \
               }                                                                                            \
                                                                                                            \
               RE_INLINE RE_V4_##SFX RE_V4_SCALE_##SFX(RE_V4_##SFX v, T s) {                              \
                   return (RE_V4_##SFX){                                                                    \
                       (T)(v.x * s), (T)(v.y * s), (T)(v.z * s), (T)(v.w * s)                               \
                   };                                                                                       \
               }                                                                                            \
                                                                                                            \
               RE_INLINE RE_V4_##SFX RE_V4_DIV_##SFX(RE_V4_##SFX v, T s) {                                \
                   if (s == (T)0) return RE_V4_ZERO_##SFX();                                              \
                   RE_f64 inv = 1.0 / (RE_f64)s;                                                            \
                   return (RE_V4_##SFX){                                                                    \
                       (T)((RE_f64)v.x * inv), (T)((RE_f64)v.y * inv),                                      \
                       (T)((RE_f64)v.z * inv), (T)((RE_f64)v.w * inv)                                       \
                   };                                                                                       \
               }                                                                                            \
                                                                                                            \
               RE_INLINE T RE_V4_DOT_##SFX(RE_V4_##SFX a, RE_V4_##SFX b) {                                \
                   RE_f64 t = (RE_f64)a.x * b.x + (RE_f64)a.y * b.y +                                       \
                              (RE_f64)a.z * b.z + (RE_f64)a.w * b.w;                                        \
                   return (T)t;                                                                             \
               }                                                                                            \
                                                                                                            \
               RE_INLINE RE_V4_##SFX RE_V4_HADAMARD_##SFX(RE_V4_##SFX a, RE_V4_##SFX b) {                 \
                   return (RE_V4_##SFX){                                                                    \
                       (T)(a.x * b.x), (T)(a.y * b.y),                                                     \
                       (T)(a.z * b.z), (T)(a.w * b.w)                                                      \
                   };                                                                                       \
               }                                                                                            \
                                                                                                            \
               RE_INLINE RE_V4_##SFX RE_V4_CLAMP_##SFX(RE_V4_##SFX v,                                     \
                                                         RE_V4_##SFX mn, RE_V4_##SFX mx) {                  \
                   return (RE_V4_##SFX){                                                                    \
                       (T)RE_CLAMP(v.x, mn.x, mx.x),                                                       \
                       (T)RE_CLAMP(v.y, mn.y, mx.y),                                                       \
                       (T)RE_CLAMP(v.z, mn.z, mx.z),                                                       \
                       (T)RE_CLAMP(v.w, mn.w, mx.w)                                                        \
                   };                                                                                       \
               }                                                                                            \
                                                                                                            \
               RE_INLINE RE_V4_##SFX RE_V4_LERP_##SFX(RE_V4_##SFX a, RE_V4_##SFX b, T t) {                \
                   RE_f64 tf = (RE_f64)t;                                                                    \
                   return (RE_V4_##SFX){                                                                     \
                       (T)(a.x + (b.x-a.x)*tf),                                                             \
                       (T)(a.y + (b.y-a.y)*tf),                                                             \
                       (T)(a.z + (b.z-a.z)*tf),                                                             \
                       (T)(a.w + (b.w-a.w)*tf)                                                              \
                   };                                                                                        \
               }

               /* floating */
               RE_GEN_V4_TYPE_AND_FUNCS(RE_f32, f32)
               RE_GEN_V4_TYPE_AND_FUNCS(RE_f64, f64)

               /* signed integers */
               RE_GEN_V4_TYPE_AND_FUNCS(RE_i8,  i8)
               RE_GEN_V4_TYPE_AND_FUNCS(RE_i16, i16)
               RE_GEN_V4_TYPE_AND_FUNCS(RE_i32, i32)
               RE_GEN_V4_TYPE_AND_FUNCS(RE_i64, i64)

               /* unsigned integers */
               RE_GEN_V4_TYPE_AND_FUNCS(RE_u8,  u8)
               RE_GEN_V4_TYPE_AND_FUNCS(RE_u16, u16)
               RE_GEN_V4_TYPE_AND_FUNCS(RE_u32, u32)
               RE_GEN_V4_TYPE_AND_FUNCS(RE_u64, u64)

               /* ============================================================================================
                  Float-only advanced math for Vec4 (f32 / f64)
                  ============================================================================================ */

               //
               // LENGTH
               //
               // RE_INLINE RE_f32 RE_V4_LENGTH_f32(RE_V4_f32 v) {
               //     RE_f32 d = v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; // outdated implementation
               //     return RE_SQRT(d);
               // }

               // RE_INLINE RE_f64 RE_V4_LENGTH_f64(RE_V4_f64 v) {
               //     RE_f64 d = v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; // outdated implementation
               //     return RE_SQRT(d);
               // }

               //
               // FAST LENGTH
               //
               RE_INLINE RE_f32 RE_V4_LENGTH_f32(RE_V4_f32 v) {
                   RE_f32 d = v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
                   return d * RE_INVSQRT(d);
               }

               RE_INLINE RE_f64 RE_V4_LENGTH_f64(RE_V4_f64 v) {
                   RE_f64 d = v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
                   return d * RE_INVSQRT(d);
               }

               //
               // NORMALIZE
               //
               RE_INLINE RE_V4_f32 RE_V4_NORMALIZE_f32(RE_V4_f32 v) {
                   RE_f32 d = v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
                   if (d <= 0.0f) return RE_V4_ZERO_f32();
                   RE_f32 inv = RE_INVSQRT(d);
                   return RE_V4_MAKE_f32(v.x*inv, v.y*inv, v.z*inv, v.w*inv);
               }

               //
               // DISTANCE
               //
               RE_INLINE RE_f32 RE_V4_DISTANCE_f32(RE_V4_f32 a, RE_V4_f32 b) {
                   return RE_V4_LENGTH_f32(RE_V4_SUB_f32(a, b));
               }

               //
               // REFLECT
               //
               RE_INLINE RE_V4_f32 RE_V4_REFLECT_f32(RE_V4_f32 I, RE_V4_f32 N) {
                   RE_f32 d = RE_V4_DOT_f32(I, N);
                   return RE_V4_MAKE_f32(
                       I.x - 2 * (d * N.x),
                       I.y - 2 * (d * N.y),
                       I.z - 2 * (d * N.z),
                       I.w - 2 * (d * N.w)
                   );
               }

               //
               // ANGLE
               //
               RE_INLINE RE_f32 RE_V4_ANGLE_f32(RE_V4_f32 A, RE_V4_f32 B) {
                   RE_f32 dot = RE_V4_DOT_f32(A, B);
                   RE_f32 lenA = RE_V4_LENGTH_f32(A);
                   RE_f32 lenB = RE_V4_LENGTH_f32(B);

                   RE_f32 d = lenA * lenB;
                   if (d <= 0.0f) return 0.0f;

                   RE_f32 c = dot / d;
                   c = RE_CLAMP(c, -1.0f, 1.0f);

                   return RE_ACOS(c);
               }

               //
               // PROJECT
               //
               RE_INLINE RE_V4_f32 RE_V4_PROJECT_f32(RE_V4_f32 A, RE_V4_f32 B) {
                   RE_f32 denom = RE_V4_DOT_f32(B, B);
                   if (denom == 0.0f) return RE_V4_ZERO_f32();

                   RE_f32 scale = RE_V4_DOT_f32(A, B) / denom;
                   return RE_V4_MAKE_f32(
                       B.x * scale,
                       B.y * scale,
                       B.z * scale,
                       B.w * scale
                   );
               }

#endif // RE_VEC_H
