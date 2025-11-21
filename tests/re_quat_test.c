    /**
     * @file re_quat_test.c
     * @brief Full unit test suite for REMath quaternion system (f32 + SIMD).
     *
     * Tests:
     *   - Identity
     *   - Make
     *   - Add
     *   - Scalar multiply
     *   - Dot (scalar + SIMD)
     *   - Length / Normalize
     *   - Axis-angle conversion
     *   - Euler -> Quaternion
     *   - Quaternion -> Euler
     *   - Hamilton product
     *   - Inverse
     *   - Conjugate
     *   - Vector rotation
     *   - SLERP
     *   - LERP
     *   - RotateTowards
     *   - Direction vectors (Forward / Right / Up)
     *
     * Author: Jayansh Devgan
     */

    #include "../include/re_quat.h"
    #include "../include/re_vec.h"
    #include "../include/re_math.h"
    #include "../include/re_math_ext.h"
    #include "../include/re_test_core.h"

    #include <math.h>
    #include <stdio.h>

    static RE_bool approx_eq_f32(RE_f32 a, RE_f32 b, RE_f32 eps)
    {
        return RE_FABS_f32(a - b) <= eps;
    }

    static RE_bool approx_vec3(RE_V3_f32 a, RE_V3_f32 b, RE_f32 eps)
    {
        return approx_eq_f32(a.x,b.x,eps) &&
               approx_eq_f32(a.y,b.y,eps) &&
               approx_eq_f32(a.z,b.z,eps);
    }

    /* ============================================================================================
       TEST: Identity
       ============================================================================================ */

    static void test_quat_identity(void)
    {
        RE_QUAT_f32 q = RE_QUAT_IDENTITY_f32();

        test_result("QUAT identity.w", approx_eq_f32(q.w, 1.f, 1e-6f));
        test_result("QUAT identity.xyz",
            approx_eq_f32(q.x,0,1e-6f) &&
            approx_eq_f32(q.y,0,1e-6f) &&
            approx_eq_f32(q.z,0,1e-6f)
        );
    }

    /* ============================================================================================
       TEST: Add and Scalar Mul
       ============================================================================================ */

    static void test_quat_add_mul(void)
    {
        RE_QUAT_f32 a = {1,2,3,4};
        RE_QUAT_f32 b = {4,3,2,1};

        RE_QUAT_f32 s = RE_QUAT_ADD_f32(a,b);
        test_result("QUAT add",
            s.x==5 && s.y==5 && s.z==5 && s.w==5);

        RE_QUAT_f32 m = RE_QUAT_MUL_SCALAR_f32(a, 2.f);
        test_result("QUAT mul scalar",
            m.x==2 && m.y==4 && m.z==6 && m.w==8);
    }

    /* ============================================================================================
       TEST: Dot + SIMD dot
       ============================================================================================ */

    static void test_quat_dot(void)
    {
        RE_QUAT_f32 a = {1,2,3,4};
        RE_QUAT_f32 b = {2,1,0,1};

        RE_f32 d_scalar = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
        RE_f32 d_simd   = RE_QUAT_DOT_f32(a,b);

        test_result("QUAT dot SIMD", approx_eq_f32(d_scalar, d_simd, 1e-6f));
    }

    /* ============================================================================================
       TEST: Length + Normalize
       ============================================================================================ */

    static void test_quat_len_norm(void)
    {
        RE_QUAT_f32 q = { 2,0,0,0 };

        RE_f32 len = RE_QUAT_LENGTH_f32(q);

        test_result("QUAT length", approx_eq_f32(len, 2.f, 1e-6f));

        RE_QUAT_f32 n = RE_QUAT_NORMALIZE_f32(q);

        test_result("QUAT normalize X",
            approx_eq_f32(n.x, 1.f, 1e-6f) &&
            approx_eq_f32(n.w, 0.f, 1e-6f));
    }

    /* ============================================================================================
       TEST: Axis-angle
       ============================================================================================ */

    static void test_axis_angle(void)
    {
        RE_V3_f32 axis = {0,0,1};
        RE_f32 angle = RE_PI_F * 0.5f;

        RE_QUAT_f32 q = RE_QUAT_FROM_AXIS_ANGLE_f32(axis, angle);

        RE_f32 s = RE_SQRT(q.x*q.x + q.y*q.y + q.z*q.z);
        test_result("AXISANGLE sin", approx_eq_f32(s, RE_SIN_f32(angle/2), 1e-3f));
        test_result("AXISANGLE cos", approx_eq_f32(q.w, RE_COS_f32(angle/2), 1e-3f));
    }

    /* ============================================================================================
       TEST: Euler conversion
       ============================================================================================ */

    static void test_from_euler_only(void)
    {
        RE_V3_f32 e = {
            RE_PI_F * 0.25f,  // 45°
            RE_PI_F * 0.50f,  // 90°
            RE_PI_F * 0.10f   // 18°
        };

        // Our function
        RE_QUAT_f32 q = RE_QUAT_FROM_EULER_f32(e);

        // Reference quaternion (Y * X * Z order)
        // Verified against Python NumPy + Unity + GLM
        float cx = RE_COS_f32(e.x * 0.5f), sx = RE_SIN_f32(e.x * 0.5f);
        float cy = RE_COS_f32(e.y * 0.5f), sy = RE_SIN_f32(e.y * 0.5f);
        float cz = RE_COS_f32(e.z * 0.5f), sz = RE_SIN_f32(e.z * 0.5f);

        RE_QUAT_f32 ref = {
            sx*cy*cz - cx*sy*sz,
            cx*sy*cz + sx*cy*sz,
            cx*cy*sz - sx*sy*cz,
            cx*cy*cz + sx*sy*sz
        };

        test_result("FROM_EULER q.x", approx_eq_f32(q.x, ref.x, 1e-4f));
        test_result("FROM_EULER q.y", approx_eq_f32(q.y, ref.y, 1e-4f));
        test_result("FROM_EULER q.z", approx_eq_f32(q.z, ref.z, 1e-4f));
        test_result("FROM_EULER q.w", approx_eq_f32(q.w, ref.w, 1e-4f));
    }

    static void test_to_euler_only(void)
    {
        struct Case {
            RE_V3_f32 e;     // input Euler
        };

        struct Case cases[] = {
            { {0, 0, 0} },
            { {RE_PI_F * 0.5f, 0, 0} },      // 90° pitch
            { {0, RE_PI_F * 0.5f, 0} },      // 90° yaw
            { {0, 0, RE_PI_F * 0.5f} },      // 90° roll
            { {RE_PI_F * 0.25f, RE_PI_F * 0.25f, RE_PI_F * 0.25f} },
            { {RE_PI_F * -0.25f, RE_PI_F * 0.1f, RE_PI_F * -0.3f} },
        };

        for (int i = 0; i < (int)(sizeof(cases)/sizeof(cases[0])); i++)
        {
            RE_V3_f32 e      = cases[i].e;
            RE_QUAT_f32 q    = RE_QUAT_FROM_EULER_f32(e);
            RE_V3_f32 back   = RE_QUAT_TO_EULER_f32(q);

            char msgX[128], msgY[128], msgZ[128];
            snprintf(msgX,128,"TO_EULER ONLY X case %d", i);
            snprintf(msgY,128,"TO_EULER ONLY Y case %d", i);
            snprintf(msgZ,128,"TO_EULER ONLY Z case %d", i);

            test_result(msgX, approx_eq_f32(e.x, back.x, 1e-3f));
            test_result(msgY, approx_eq_f32(e.y, back.y, 1e-3f));
            test_result(msgZ, approx_eq_f32(e.z, back.z, 1e-3f));
        }
    }

    static void test_euler_conversion(void)
    {
        RE_V3_f32 e = { RE_PI_F*0.25f, RE_PI_F*0.5f, RE_PI_F*0.1f };

        RE_QUAT_f32 q = RE_QUAT_FROM_EULER_f32(e);
        RE_V3_f32 back = RE_QUAT_TO_EULER_f32(q);

        char nameX[128], nameY[128], nameZ[128];
        snprintf(nameX, sizeof(nameX), "EULER->QUAT->EULER X  (%f)", e.x);
        snprintf(nameY, sizeof(nameY), "EULER->QUAT->EULER Y  (%f)", e.y);
        snprintf(nameZ, sizeof(nameZ), "EULER->QUAT->EULER Z  (%f)", e.z);

        test_result(nameX, approx_eq_f32(e.x, back.x, 1e-3f));
        test_result(nameY, approx_eq_f32(e.y, back.y, 1e-3f));
        test_result(nameZ, approx_eq_f32(e.z, back.z, 1e-3f));
    }

    /* ============================================================================================
       TEST: Hamilton product
       ============================================================================================ */

    static void test_hamilton_mul(void)
    {
        RE_QUAT_f32 a = {0,0, RE_SIN_f32(0.25f), RE_COS_f32(0.25f)};
        RE_QUAT_f32 b = {0,0, RE_SIN_f32(0.50f), RE_COS_f32(0.50f)};

        RE_QUAT_f32 ab = RE_QUAT_MUL_f32(a,b);

        RE_f32 expected_angle = 0.25f + 0.50f;
        RE_f32 expected_cos = RE_COS_f32(expected_angle);
        test_result("HAM MUL cos", approx_eq_f32(ab.w, expected_cos, 1e-3f));
    }

    /* ============================================================================================
       TEST: Conjugate + Inverse
       ============================================================================================ */

    static void test_conjugate_inverse(void)
    {
        RE_QUAT_f32 q = {1,2,3,4};

        RE_QUAT_f32 c = RE_QUAT_CONJUGATE_f32(q);
        test_result("CONJUGATE", c.x==-1 && c.y==-2 && c.z==-3 && c.w==4);

        RE_QUAT_f32 inv = RE_QUAT_INVERSE_f32(q);
        RE_f32 len2 = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;

        test_result("INVERSE len2", approx_eq_f32(inv.x, -q.x/len2, 1e-6f));
    }

    /* ============================================================================================
       TEST: Rotate vector
       ============================================================================================ */

    static void test_rotate_vec3(void)
    {
        RE_QUAT_f32 q = RE_QUAT_FROM_AXIS_ANGLE_f32((RE_V3_f32){0,1,0}, RE_PI_F * 0.5f);

        RE_V3_f32 f = {0,0,-1};
        RE_V3_f32 r = RE_QUAT_ROTATE_VEC3_f32(q,f);

        /* Should become (-1,0,0) */
        test_result("ROTATE VEC3",
            approx_vec3(r, (RE_V3_f32){-1,0,0}, 1e-3f));
    }

    /* ============================================================================================
       TEST: SLERP
       ============================================================================================ */

    static void test_slerp(void)
    {
        RE_QUAT_f32 a = RE_QUAT_IDENTITY_f32();
        RE_QUAT_f32 b = RE_QUAT_FROM_AXIS_ANGLE_f32((RE_V3_f32){0,1,0}, RE_PI_F);

        RE_QUAT_f32 h = RE_QUAT_SLERP_f32(a,b,0.5f);

        test_result("SLERP w positive", h.w <= 1.f && h.w >= 0.f);
    }

    /* ============================================================================================
       TEST: LERP
       ============================================================================================ */

    static void test_lerp(void)
    {
        RE_QUAT_f32 a = {0,0,0,1};
        RE_QUAT_f32 b = {0,1,0,0};

        RE_QUAT_f32 r = RE_QUAT_LERP_f32(a,b,0.5f);

        test_result("LERP y", approx_eq_f32(r.y, 0.5f, 1e-6f));
    }

    /* ============================================================================================
       TEST: RotateTowards
       ============================================================================================ */

    static void test_rotate_towards(void)
    {
        RE_QUAT_f32 a = RE_QUAT_IDENTITY_f32();
        RE_QUAT_f32 b = RE_QUAT_FROM_AXIS_ANGLE_f32((RE_V3_f32){0,1,0}, RE_PI_F);

        RE_QUAT_f32 r = RE_QUAT_ROTATE_TOWARDS_f32(a,b, RE_PI_F * 0.1f);

        test_result("ROTATE TOWARDS w", r.w > 0.9f);
    }

    /* ============================================================================================
       TEST: Direction Vectors
       ============================================================================================ */

    static void test_directions(void)
    {
        RE_QUAT_f32 q = RE_QUAT_FROM_AXIS_ANGLE_f32((RE_V3_f32){0,1,0}, RE_PI_F * 0.5f);

        RE_V3_f32 f = RE_QUAT_FORWARD_f32(q);   /* expected (-1,0,0) */
        RE_V3_f32 r = RE_QUAT_RIGHT_f32(q);     /* expected (0,0,-1) */
        RE_V3_f32 u = RE_QUAT_UP_f32(q);        /* expected (0,1,0) */

        test_result("DIR forward", approx_vec3(f,(RE_V3_f32){-1,0,0},1e-3f));
        test_result("DIR right",   approx_vec3(r,(RE_V3_f32){0,0,-1},1e-3f));
        test_result("DIR up",      approx_vec3(u,(RE_V3_f32){0,1,0},1e-3f));
    }

    /* ============================================================================================
       RUN ALL TESTS
       ============================================================================================ */

    void run_quat_tests(void)
    {
        printf("=== quaternion tests start ===\n");

        test_quat_identity();
        test_quat_add_mul();
        test_quat_dot();
        test_quat_len_norm();
        test_axis_angle();
        // test_from_euler_only();
        // test_to_euler_only();
        // test_euler_conversion();
        test_hamilton_mul();
        test_conjugate_inverse();
        test_rotate_vec3();
        test_slerp();
        test_lerp();
        test_rotate_towards();
        test_directions();

        printf("=== quaternion tests finished ===\n");
    }
