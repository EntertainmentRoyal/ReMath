#ifndef RE_QUAT_H
#define RE_QUAT_H

#include "re_core.h"
#include "re_math.h"
#include "re_math_ext.h"
#include "re_vec.h"

/* ================================================================
   QUAT TYPES
   ================================================================ */

typedef struct { RE_f32 x, y, z, w; } RE_QUAT_f32;
typedef struct { RE_f64 x, y, z, w; } RE_QUAT_f64;


/* ================================================================
   QUAT CONSTRUCTORS
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_MAKE_f32(RE_f32 x, RE_f32 y, RE_f32 z, RE_f32 w)
{
    RE_QUAT_f32 q = { x,y,z,w };
    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_MAKE_f64(RE_f64 x, RE_f64 y, RE_f64 z, RE_f64 w)
{
    RE_QUAT_f64 q = { x,y,z,w };
    return q;
}

RE_INLINE RE_QUAT_f32 RE_QUAT_IDENTITY_f32(void)
{
    RE_QUAT_f32 q = {0,0,0,1};
    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_IDENTITY_f64(void)
{
    RE_QUAT_f64 q = {0,0,0,1};
    return q;
}


/* ================================================================
   BASIC OPS
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_ADD_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    RE_QUAT_f32 q = {
        a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w
    };
    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_ADD_f64(RE_QUAT_f64 a, RE_QUAT_f64 b)
{
    RE_QUAT_f64 q = {
        a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w
    };
    return q;
}

RE_INLINE RE_QUAT_f32 RE_QUAT_MUL_SCALAR_f32(RE_QUAT_f32 q, RE_f32 s)
{
    RE_QUAT_f32 r = { q.x*s, q.y*s, q.z*s, q.w*s };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_MUL_SCALAR_f64(RE_QUAT_f64 q, RE_f64 s)
{
    RE_QUAT_f64 r = { q.x*s, q.y*s, q.z*s, q.w*s };
    return r;
}


/* ================================================================
   HAMILTON PRODUCT (q1 * q2)
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_MUL_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    RE_QUAT_f32 q;

    q.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
    q.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
    q.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
    q.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;

    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_MUL_f64(RE_QUAT_f64 a, RE_QUAT_f64 b)
{
    RE_QUAT_f64 q;

    q.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
    q.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
    q.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
    q.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;

    return q;
}


/* ================================================================
   LENGTH / NORMALIZE
   ================================================================ */

RE_INLINE RE_f32 RE_QUAT_LENGTH_f32(RE_QUAT_f32 q)
{
    return RE_SQRT(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

RE_INLINE RE_f64 RE_QUAT_LENGTH_f64(RE_QUAT_f64 q)
{
    return RE_SQRT(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

RE_INLINE RE_QUAT_f32 RE_QUAT_NORMALIZE_f32(RE_QUAT_f32 q)
{
    RE_f32 l = RE_QUAT_LENGTH_f32(q);
    if (l <= 0.0f) return RE_QUAT_IDENTITY_f32();
    RE_f32 inv =  1.0f / l;

    RE_QUAT_f32 r = { q.x*inv, q.y*inv, q.z*inv, q.w*inv };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_NORMALIZE_f64(RE_QUAT_f64 q)
{
    RE_f64 l = RE_QUAT_LENGTH_f64(q);
    if (l <= 0.0f) return RE_QUAT_IDENTITY_f64();
    RE_f64 inv = 1.0 / l;

    RE_QUAT_f64 r = { q.x*inv, q.y*inv, q.z*inv, q.w*inv };
    return r;
}


/* ================================================================
   FROM AXIS-ANGLE
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_FROM_AXIS_ANGLE_f32(RE_V3_f32 axis, RE_f32 angle)
{
    RE_f32 s = RE_SIN_f32(0.5f * angle);
    RE_f32 c = RE_COS_f32(0.5f * angle);

    RE_V3_f32 na = RE_V3_NORMALIZE_f32(axis);

    RE_QUAT_f32 q = { na.x*s, na.y*s, na.z*s, c };
    return q;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_FROM_AXIS_ANGLE_f64(RE_V3_f64 axis, RE_f64 angle)
{
    RE_f64 s = RE_SIN_f32(angle * 0.5);
    RE_f64 c = RE_COS_f32(angle * 0.5);

    RE_f64 len = RE_SQRT(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
    if (len == 0.0) return RE_QUAT_IDENTITY_f64();

    RE_f64 inv = 1.0 / len;

    RE_QUAT_f64 q = {
        axis.x*inv*s,
        axis.y*inv*s,
        axis.z*inv*s,
        c
    };
    return q;
}


/* ================================================================
   QUAT → MAT4
   (Used by your TRS and LookAt)
   ================================================================ */

RE_INLINE void RE_QUAT_TO_MAT4_f32(RE_QUAT_f32 q, RE_f32 out[16])
{
    RE_f32 x=q.x, y=q.y, z=q.z, w=q.w;

    RE_f32 xx=x*x, yy=y*y, zz=z*z;
    RE_f32 xy=x*y, xz=x*z, yz=y*z;
    RE_f32 wx=w*x, wy=w*y, wz=w*z;

    out[ 0] = 1 - 2*(yy + zz);
    out[ 1] = 2*(xy + wz);
    out[ 2] = 2*(xz - wy);
    out[ 3] = 0;

    out[ 4] = 2*(xy - wz);
    out[ 5] = 1 - 2*(xx + zz);
    out[ 6] = 2*(yz + wx);
    out[ 7] = 0;

    out[ 8] = 2*(xz + wy);
    out[ 9] = 2*(yz - wx);
    out[10] = 1 - 2*(xx + yy);
    out[11] = 0;

    out[12]=0; out[13]=0; out[14]=0; out[15]=1;
}

RE_INLINE void RE_QUAT_TO_MAT4_f64(RE_QUAT_f64 q, RE_f64 out[16])
{
    RE_f64 x=q.x, y=q.y, z=q.z, w=q.w;

    RE_f64 xx=x*x, yy=y*y, zz=z*z;
    RE_f64 xy=x*y, xz=x*z, yz=y*z;
    RE_f64 wx=w*x, wy=w*y, wz=w*z;

    out[ 0] = 1 - 2*(yy + zz);
    out[ 1] = 2*(xy + wz);
    out[ 2] = 2*(xz - wy);
    out[ 3] = 0;

    out[ 4] = 2*(xy - wz);
    out[ 5] = 1 - 2*(xx + zz);
    out[ 6] = 2*(yz + wx);
    out[ 7] = 0;

    out[ 8] = 2*(xz + wy);
    out[ 9] = 2*(yz - wx);
    out[10] = 1 - 2*(xx + yy);
    out[11] = 0;

    out[12]=0; out[13]=0; out[14]=0; out[15]=1;
}


/* ================================================================
   SLERP (Stable, branch-minimized)
   ================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_SLERP_f32(RE_QUAT_f32 a, RE_QUAT_f32 b, RE_f32 t)
{
    RE_f32 dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;

    /* ensure shortest path */
    if (dot < 0.0f) {
        dot = -dot;
        b.x=-b.x; b.y=-b.y; b.z=-b.z; b.w=-b.w;
    }

    const RE_f32 DOT_T = 0.9995f;

    if (dot > DOT_T)
    {
        /* linear fallback */
        RE_QUAT_f32 q = {
            a.x + t*(b.x-a.x),
            a.y + t*(b.y-a.y),
            a.z + t*(b.z-a.z),
            a.w + t*(b.w-a.w)
        };
        return RE_QUAT_NORMALIZE_f32(q);
    }

    RE_f32 th = RE_ACOS(dot);
    RE_f32 s = RE_RCP(RE_SIN_f32(th));

    RE_f32 t0 = RE_SIN_f32((1.0f-t)*th) * s;
    RE_f32 t1 = RE_SIN_f32(t*th) * s;

    RE_QUAT_f32 q = {
        a.x*t0 + b.x*t1,
        a.y*t0 + b.y*t1,
        a.z*t0 + b.z*t1,
        a.w*t0 + b.w*t1
    };
    return q;
}

RE_INLINE RE_V3_f32 RE_QUAT_ROTATE_VEC3_f32(RE_QUAT_f32 q, RE_V3_f32 v)
{
    RE_f32 len2 = RE_QUAT_LENGTH_f32(q);
    if (len2 < 1e-12f)
        return v;

    RE_f32 inv_len = 1.0f / RE_SQRT(len2);

    RE_f32 x = q.x * inv_len;
    RE_f32 y = q.y * inv_len;
    RE_f32 z = q.z * inv_len;
    RE_f32 w = q.w * inv_len;

    // t = 2 * cross(q.xyz, v)
    RE_V3_f32 t;
    t.x = 2.0f * (y*v.z - z*v.y);
    t.y = 2.0f * (z*v.x - x*v.z);
    t.z = 2.0f * (x*v.y - y*v.x);

    // v' = v + w*t + cross(q.xyz, t)
    RE_V3_f32 r;

    // w*t
    RE_V3_f32 wt = { w*t.x, w*t.y, w*t.z };

    // cross(q.xyz, t)
    RE_V3_f32 c;
    c.x = y*t.z - z*t.y;
    c.y = z*t.x - x*t.z;
    c.z = x*t.y - y*t.x;

    r.x = v.x + wt.x + c.x;
    r.y = v.y + wt.y + c.y;
    r.z = v.z + wt.z + c.z;

    return r;
}

RE_INLINE RE_QUAT_f32 RE_QUAT_CONJUGATE_f32(RE_QUAT_f32 q)
{
    RE_QUAT_f32 r = { -q.x, -q.y, -q.z, q.w };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_CONJUGATE_f64(RE_QUAT_f64 q)
{
    RE_QUAT_f64 r = { -q.x, -q.y, -q.z, q.w };
    return r;
}

/* --------------------------
   Inverse q⁻¹ = q* / |q|²
   -------------------------- */
RE_INLINE RE_QUAT_f32 RE_QUAT_INVERSE_f32(RE_QUAT_f32 q)
{
    RE_f32 len2 =
        q.x*q.x + q.y*q.y +
        q.z*q.z + q.w*q.w;

    if (len2 <= 1e-12f)
        return RE_QUAT_IDENTITY_f32();

    RE_f32 inv = RE_RCP(len2);

    RE_QUAT_f32 r;
    r.x = -q.x * inv;
    r.y = -q.y * inv;
    r.z = -q.z * inv;
    r.w =  q.w * inv;
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_INVERSE_f64(RE_QUAT_f64 q)
{
    RE_f64 len2 = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    if (len2 <= 0.0) return RE_QUAT_IDENTITY_f64();
    RE_f64 inv = 1.0 / len2;

    RE_QUAT_f64 r = { -q.x * inv, -q.y * inv, -q.z * inv, q.w * inv };
    return r;
}


/* ============================================================================
   AXIS ROTATION
   ============================================================================ */

RE_INLINE RE_QUAT_f32
RE_QUAT_ROTATE_AXIS_f32(RE_V3_f32 axis, RE_f32 angle_rad)
{
    RE_V3_f32 n = RE_V3_NORMALIZE_f32(axis);
    RE_f32 s = RE_SIN_f32(angle_rad * 0.5f);

    RE_QUAT_f32 q = { n.x*s, n.y*s, n.z*s, RE_COS_f32(angle_rad * 0.5f) };
    return q;
}

/* ============================================================================
   EULER → QUATERNION (XYZ order — standard engine order)
   ============================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_FROM_EULER_f32(RE_V3_f32 e)
{
    RE_QUAT_f32 q;

    RE_f32 len = RE_SQRT(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
        if (len > 0.0f) {
            RE_f32 inv = 1.0f / len;
            q.x *= inv; q.y *= inv; q.z *= inv; q.w *= inv;
        }

        RE_f32 sinp = 2.0f * (q.w*q.x + q.y*q.z);
        RE_f32 cosp = 1.0f - 2.0f * (q.x*q.x + q.y*q.y);
        e.x = RE_ATAN2_f32(sinp, cosp);

        RE_f32 siny = 2.0f * (q.w*q.y - q.z*q.x);
        if (RE_FABS_f32(siny) >= 1)
            e.y = RE_COPYSIGN_f32(RE_PI_F/2.0f, siny);
        else
            e.y = RE_ASIN(siny);

        RE_f32 sinr = 2.0f * (q.w*q.z + q.x*q.y);
        RE_f32 cosr = 1.0f - 2.0f * (q.y*q.y + q.z*q.z);
        e.z = RE_ATAN2_f32(sinr, cosr);

        return q;
}

RE_INLINE RE_QUAT_f32 RE_QUAT_FROM_EULER_f64(RE_V3_f64 e)
{
    RE_f64 hx = e.x * 0.5f;
    RE_f64 hy = e.y * 0.5f;
    RE_f64 hz = e.z * 0.5f;

    RE_f32 cx = RE_COS_f32(hx), sx = RE_SIN_f32(hx);
    RE_f32 cy = RE_COS_f32(hy), sy = RE_SIN_f32(hy);
    RE_f32 cz = RE_COS_f32(hz), sz = RE_SIN_f32(hz);

    RE_QUAT_f32 q;

    // XYZ intrinsic rotation (Rz * Ry * Rx)
    q.w = cx*cy*cz + sx*sy*sz;
    q.x = sx*cy*cz - cx*sy*sz;
    q.y = cx*sy*cz + sx*cy*sz;
    q.z = cx*cy*sz - sx*sy*cz;

    return q;
}


/* ============================================================================
   QUATERNION → EULER
   ============================================================================ */

RE_INLINE RE_V3_f32 RE_QUAT_TO_EULER_f32(RE_QUAT_f32 q)
{
    RE_V3_f32 e;

    RE_f32 len = RE_SQRT(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
    if (len > 0.0f) {
        RE_f32 inv = 1.0f / len;
        q.x *= inv; q.y *= inv; q.z *= inv; q.w *= inv;
    }

    // ZYX extraction (roll, yaw, pitch)
    // pitch (X)
    RE_f32 sinp = 2.0f * (q.w*q.x + q.y*q.z);
    RE_f32 cosp = 1.0f - 2.0f * (q.x*q.x + q.y*q.y);
    e.x = RE_ATAN2_f32(sinp, cosp);

    // yaw (Y)
    RE_f32 siny = 2.0f * (q.w*q.y - q.z*q.x);
    if (RE_FABS_f32(siny) >= 1.0f)
            e.y = RE_COPYSIGN_f32(RE_PI_F * 0.5f, siny); // clamp
    else e.y = RE_ASIN(siny);

    // roll (Z)
    RE_f32 sinr = 2.0f * (q.w*q.z + q.x*q.y);
    RE_f32 cosr = 1.0f - 2.0f * (q.y*q.y + q.z*q.z);
    e.z = RE_ATAN2_f32(sinr, cosr);

    return e;
}

RE_INLINE RE_V3_f64 RE_QUAT_TO_EULER_f64(RE_QUAT_f64 q)
{
    RE_V3_f64 e;

    RE_f64 len = RE_SQRT(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
    if (len > 0.0f) {
        RE_f64 inv = 1.0f / len;
        q.x *= inv; q.y *= inv; q.z *= inv; q.w *= inv;
    }

    RE_f64 sp = 2.0f * (q.w*q.x - q.y*q.z);
    if (sp >= 1.0f)
        e.x = RE_PI_F * 0.5f;   // +90°
    else if (sp <= -1.0f)
        e.x = -RE_PI_F * 0.5f;  // -90°
    else
        e.x = RE_ASIN(sp);

    e.y = RE_ATAN2_f32(
        2.0f * (q.w*q.y + q.z*q.x),
        1.0f - 2.0f * (q.x*q.x + q.y*q.y)
    );

    e.z = RE_ATAN2_f32(
        2.0f * (q.w*q.z + q.x*q.y),
        1.0f - 2.0f * (q.x*q.x + q.z*q.z)
    );

    return e;
}


/* ============================================================================
   LERP
   ============================================================================ */

RE_INLINE RE_QUAT_f32 RE_QUAT_LERP_f32(RE_QUAT_f32 a, RE_QUAT_f32 b, RE_f32 t)
{
    RE_QUAT_f32 r = {
        a.x + (b.x - a.x)*t,
        a.y + (b.y - a.y)*t,
        a.z + (b.z - a.z)*t,
        a.w + (b.w - a.w)*t
    };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_LERP_f64(RE_QUAT_f64 a, RE_QUAT_f64 b, RE_f64 t)
{
    RE_QUAT_f64 r = {
        a.x + (b.x - a.x)*t,
        a.y + (b.y - a.y)*t,
        a.z + (b.z - a.z)*t,
        a.w + (b.w - a.w)*t
    };
    return r;
}

RE_INLINE RE_QUAT_f64 RE_QUAT_SLERP_f64(RE_QUAT_f64 a, RE_QUAT_f64 b, RE_f64 t)
{
    RE_f64 dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;

    if (dot < 0.0) {
        b.x *= -1; b.y *= -1; b.z *= -1; b.w *= -1;
        dot = -dot;
    }

    if (dot > 0.9995)
        return RE_QUAT_INVERSE_f64(RE_QUAT_LERP_f64(a, b, t));

    RE_f64 theta = RE_ACOS(dot);
    RE_f64 s = RE_SIN_f32(theta);

    RE_f64 w1 = RE_SIN_f32((1-t)*theta) / s;
    RE_f64 w2 = RE_SIN_f32(t*theta) / s;

    RE_QUAT_f64 r = {
        a.x*w1 + b.x*w2,
        a.y*w1 + b.y*w2,
        a.z*w1 + b.z*w2,
        a.w*w1 + b.w*w2
    };
    return r;
}

RE_INLINE RE_f32 RE_QUAT_DOT_f32(RE_QUAT_f32 a, RE_QUAT_f32 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

/* ============================================================================
   ROTATE TOWARDS (unity-compatible)
   ============================================================================ */

RE_INLINE RE_QUAT_f32
RE_QUAT_ROTATE_TOWARDS_f32(RE_QUAT_f32 a, RE_QUAT_f32 b, RE_f32 max_rad)
{
    RE_f32 dot = RE_QUAT_DOT_f32(a, b);

    if (dot > 0.999999f)
        return b;

    RE_f32 angle = RE_ACOS(dot);

    RE_f32 t = RE_MIN_f32(1.0f, max_rad / angle);

    return RE_QUAT_SLERP_f32(a, b, t);
}


/* ============================================================================
   GET DIRECTIONS (Forward / Right / Up)
   ============================================================================ */

RE_INLINE RE_V3_f32 RE_QUAT_FORWARD_f32(RE_QUAT_f32 q)
{
    return RE_QUAT_ROTATE_VEC3_f32(q, (RE_V3_f32){0,0,-1});
}

RE_INLINE RE_V3_f32 RE_QUAT_RIGHT_f32(RE_QUAT_f32 q)
{
    return RE_QUAT_ROTATE_VEC3_f32(q, (RE_V3_f32){1,0,0});
}

RE_INLINE RE_V3_f32 RE_QUAT_UP_f32(RE_QUAT_f32 q)
{
    return RE_QUAT_ROTATE_VEC3_f32(q, (RE_V3_f32){0,1,0});
}

#endif /* RE_QUAT_H */
