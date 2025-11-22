/**
 * @file re_mat_test.c
 * @brief Unit tests for RE Math Matrix Library (2x2, 3x3, 4x4, SIMD).
 *
 * Mirrors the style of re_core_test.c.
 *
 * Tests cover:
 *  - constructors
 *  - zero / identity
 *  - get/set
 *  - determinant
 *  - transpose
 *  - multiply (self & reference)
 *  - inverse (including A * inv(A) = I)
 *  - SIMD vs scalar equivalence
 *
 * Author: Jayansh Devgan
 */

#include "../include/re_core.h"
#include "../include/re_math.h"
#include "../include/re_mat.h"
#include "../include/re_test_core.h"
#include "../include/re_constants.h"

#include <stdio.h>
#include <assert.h>
#include <math.h>

/* ============================================================================================
   HELPERS
   ============================================================================================ */

static RE_BOOL approx_eq_f32(RE_f32 a, RE_f32 b, RE_f32 eps) {
    RE_f32 d = a - b; if (d < 0) d = -d; return d <= eps;
}
static RE_BOOL approx_eq_f64(RE_f64 a, RE_f64 b, RE_f64 eps) {
    RE_f64 d = a - b; if (d < 0) d = -d; return d <= eps;
}

static RE_BOOL mat2_eq_f32(const RE_M2_F32 *A, const RE_M2_F32 *B, RE_f32 eps) {
    for(int i=0;i<4;i++) if(!approx_eq_f32(A->m[i],B->m[i],eps)) return RE_FALSE;
    return RE_TRUE;
}
static RE_BOOL mat2_eq_f64(const RE_M2_F64 *A, const RE_M2_F64 *B, RE_f64 eps) {
    for(int i=0;i<4;i++) if(!approx_eq_f64(A->m[i],B->m[i],eps)) return RE_FALSE;
    return RE_TRUE;
}

static RE_BOOL mat3_eq_f32(const RE_M3_F32 *A, const RE_M3_F32 *B, RE_f32 eps) {
    for(int i=0;i<9;i++) if(!approx_eq_f32(A->m[i],B->m[i],eps)) return RE_FALSE;
    return RE_TRUE;
}
static RE_BOOL mat3_eq_f64(const RE_M3_F64 *A, const RE_M3_F64 *B, RE_f64 eps) {
    for(int i=0;i<9;i++) if(!approx_eq_f64(A->m[i],B->m[i],eps)) return RE_FALSE;
    return RE_TRUE;
}

static RE_BOOL mat4_eq_f32(const RE_M4_F32 *A, const RE_M4_F32 *B, RE_f32 eps) {
    for(int i=0;i<16;i++) if(!approx_eq_f32(A->m[i],B->m[i],eps)) return RE_FALSE;
    return RE_TRUE;
}
static RE_BOOL mat4_eq_f64(const RE_M4_F64 *A, const RE_M4_F64 *B, RE_f64 eps) {
    for(int i=0;i<16;i++) if(!approx_eq_f64(A->m[i],B->m[i],eps)) return RE_FALSE;
    return RE_TRUE;
}

/* ============================================================================================
   M2 TESTS
   ============================================================================================ */

static void test_m2(void)
{
    /* Constructors */
    RE_M2_F32 A = RE_M2F32_MAKE(1,2,3,4);
    test_result("M2F32 constructor",
        A.m[0]==1 && A.m[1]==2 && A.m[2]==3 && A.m[3]==4);

    RE_M2_F64 A64 = RE_M2F64_MAKE(1,2,3,4);
    test_result("M2F64 constructor",
        A64.m[0]==1 && A64.m[1]==2 && A64.m[2]==3 && A64.m[3]==4);

    /* Zero / Identity */
    RE_M2_F32 Z = RE_M2F32_ZERO();
    RE_M2_F32 I = RE_M2F32_IDENTITY();
    test_result("M2F32 zero", Z.m[0]==0 && Z.m[3]==0);
    test_result("M2F32 identity", I.m[0]==1 && I.m[3]==1);

    /* Get/Set */
    RE_M2_F32 B = RE_M2F32_ZERO();
    RE_M2F32_SET(&B,1,1,5.f);
    RE_BOOL getset_ok = (RE_M2F32_GET(&B,1,1)==5.f);
    test_result("M2F32 get/set", getset_ok);

    /* Determinant */
    RE_f32 det = RE_M2F32_DET(&A);
    test_result("M2F32 det", approx_eq_f32(det, -2.0f, 1e-6f));

    /* Multiply */
    RE_M2_F32 C = RE_M2F32_MULTIPLY(&A,&I);
    test_result("M2F32 multiply identity", mat2_eq_f32(&A,&C,1e-6f));

    /* Inverse */
    RE_M2_F32 inv;
    RE_BOOL ok = RE_M2F32_INVERSE(&inv,&A);
    test_result("M2F32 inverse detected invertible", ok);

    RE_M2_F32 AinvA = RE_M2F32_MULTIPLY(&A,&inv);
    test_result("M2F32 A * inv(A) = I", mat2_eq_f32(&AinvA,&I,1e-4f));

    /* Same tests for F64 */
    RE_f64 det64 = RE_M2F64_DET(&A64);
    test_result("M2F64 det", approx_eq_f64(det64, -2.0, 1e-12));

    RE_M2_F64 inv64;
    ok = RE_M2F64_INVERSE(&inv64,&A64);
    test_result("M2F64 inverse detected invertible", ok);

    RE_M2_F64 AinvA64 = RE_M2F64_MULTIPLY(&A64,&inv64);
    const RE_M2_F64 output =  RE_M2F64_IDENTITY();
    test_result("M2F64 A * inv(A) = I",
        mat2_eq_f64(&AinvA64,&output,1e-12));
}

/* ============================================================================================
   M3 TESTS
   ============================================================================================ */

static void test_m3(void)
{
    RE_M3_F32 A = RE_M3F32_MAKE(
        1,2,3,
        4,5,6,
        7,8,9
    );

    /* Constructors */
    test_result("M3F32 ctor",
        A.m[0]==1 && A.m[4]==5 && A.m[8]==9);

    /* Zero / IDENTITY */
    RE_M3_F32 I = RE_M3F32_IDENTITY();
    test_result("M3F32 identity",
        I.m[0]==1 && I.m[4]==1 && I.m[8]==1);

    /* Determinant */
    RE_f32 det = RE_M3F32_DET(&A);
    test_result("M3F32 det", approx_eq_f32(det,0,1e-6f)); /* Known singular */

    /* Multiply */
    RE_M3_F32 I2 = RE_M3F32_MULTIPLY(&I,&I);
    test_result("M3F32 I*I = I", mat3_eq_f32(&I,&I2,1e-6f));

    /* Inverse (non-invertible A) */
    RE_M3_F32 inv;
    RE_BOOL ok = RE_M3F32_INVERSE(&inv,&A);
    test_result("M3F32 inverse of singular yields false", !ok);

    /* Test a known invertible matrix */
    RE_M3_F32 B = RE_M3F32_MAKE(
        2,0,1,
        1,1,0,
        0,3,1
    );
    ok = RE_M3F32_INVERSE(&inv,&B);
    test_result("M3F32 invertible detection", ok);

    RE_M3_F32 BinvB = RE_M3F32_MULTIPLY(&B,&inv);
    test_result("M3F32 B*inv(B)=I", mat3_eq_f32(&BinvB,&I,1e-4f));

    /* ========== F64 version ========== */
    RE_M3_F64 BF64 = RE_M3F64_MAKE(
        2,0,1,
        1,1,0,
        0,3,1
    );
    RE_M3_F64 inv64;
    ok = RE_M3F64_INVERSE(&inv64,&BF64);
    test_result("M3F64 invertible detection", ok);

    RE_M3_F64 BF64inv = RE_M3F64_MULTIPLY(&BF64,&inv64);
    const RE_M3_F64 output =  RE_M3F64_IDENTITY();
    test_result("M3F64 B*inv(B)=I",
        mat3_eq_f64(&BF64inv,&output,1e-12));
}

/* ============================================================================================
   M4 TESTS
   ============================================================================================ */

static void test_m4_basic(void)
{
    RE_M4_F32 I = RE_M4F32_IDENTITY();
    test_result("M4F32 identity", I.m[0]==1 && I.m[5]==1 && I.m[10]==1 && I.m[15]==1);

    RE_M4_F32 Z = RE_M4F32_ZERO();
    test_result("M4F32 zero", Z.m[0]==0 && Z.m[15]==0);

    RE_M4_F32 M = RE_M4F32_MAKE(
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,15,16
    );
    test_result("M4F32 constructor", M.m[0]==1 && M.m[5]==6 && M.m[15]==16);

    /* Transpose */
    RE_M4_F32 T = RE_M4F32_TRANSPOSE(&M);
    test_result("M4F32 transpose",
        T.m[0]  == 1  &&
                T.m[4]  == 2  &&
                T.m[8]  == 3  &&
                T.m[12] == 4  &&
                T.m[1]  == 5  &&
                T.m[5]  == 6  &&
                T.m[9]  == 7  &&
                T.m[13] == 8  &&
                T.m[2]  == 9  &&
                T.m[6]  == 10 &&
                T.m[10] == 11 &&
                T.m[14] == 12 &&
                T.m[3]  == 13 &&
                T.m[7]  == 14 &&
                T.m[11] == 15 &&
                T.m[15] == 16);
}

static void test_m4_mul(void)
{
    RE_M4_F32 A = RE_M4F32_MAKE(
        1,0,0,0,
        0,2,0,0,
        0,0,3,0,
        0,0,0,1
    );
    RE_M4_F32 B = RE_M4F32_MAKE(
        2,3,4,5,
        6,7,8,9,
        1,1,1,1,
        0,0,0,1
    );

    RE_M4_F32 C = RE_M4F32_MULTIPLY(&A,&B);

    test_result("M4F32 multiply scale-diagonal",
        approx_eq_f32(C.m[0],2,1e-6f) &&
        approx_eq_f32(C.m[5],14,1e-6f) &&
        approx_eq_f32(C.m[10],3,1e-6f));
}

static void test_m4_inverse(void)
{
    RE_M4_F32 A = RE_M4F32_MAKE(
        4,7,2,3,
        0,5,0,1,
        0,0,3,0,
        0,0,0,1
    );

    RE_M4_F32 inv;
    RE_BOOL ok = RE_M4F32_INVERSE(&inv,&A);
    test_result("M4F32 inverse exists", ok);

    RE_M4_F32 I = RE_M4F32_IDENTITY();
    RE_M4_F32 AI = RE_M4F32_MULTIPLY(&A,&inv);

    test_result("M4F32 A*inv(A)=I", mat4_eq_f32(&AI,&I,1e-4f));
}

static void test_m4_det(void)
{
    RE_M4_F32 A = RE_M4F32_MAKE(
         6, 1, 1, 0,
         4,-2, 5, 0,
         2, 8, 7, 0,
         0, 0, 0, 1
    );

    RE_f32 d = RE_M4F32_DET(&A);
    test_result("M4F32 determinant",
        approx_eq_f32(d, -306.0f, 1e-3f));
}

/* ============================================================================================
   M4 Transform Tests (TRS, LOOKAT, PERSPECTIVE, ORTHO)
   ============================================================================================ */

static void test_m4_trs(void)
{
    RE_V3_f32 T = {1,2,3};
    RE_V4_f32 Q = {0,0,0,1}; /* identity rotation */
    RE_V3_f32 S = {1,1,1};

    RE_M4_F32 M = RE_M4F32_TRS(T,Q,S);

    test_result("M4F32 TRS translation",
        approx_eq_f32(M.m[12],1,1e-6f) &&
        approx_eq_f32(M.m[13],2,1e-6f) &&
        approx_eq_f32(M.m[14],3,1e-6f));

    test_result("M4F32 TRS rotation identity",
        approx_eq_f32(M.m[0],1,1e-6f) &&
        approx_eq_f32(M.m[5],1,1e-6f) &&
        approx_eq_f32(M.m[10],1,1e-6f));
}

static void test_m4_lookat(void)
{
    RE_V3_f32 eye = {0.0f,  0.0f,  5.0f};
    RE_V3_f32 tgt = {0.0f,  0.0f,  0.0f};
    RE_V3_f32 up  = {0.0f,  1.0f,  0.0f};

    RE_M4_F32 M = RE_M4F32_LOOKAT_RH(eye, tgt, up);

    test_result("LookAt forward.x", approx_eq_f32(M.m[8],  0.0f, 1e-4f));
    test_result("LookAt forward.y", approx_eq_f32(M.m[9],  0.0f, 1e-4f));
    test_result("LookAt forward.z", approx_eq_f32(M.m[10], 1.0f, 1e-4f));

    test_result("LookAt translation z", approx_eq_f32(M.m[14], -5.0f, 1e-4f));
}

static void test_m4_perspective(void)
{
    RE_M4_F32 P = RE_M4F32_PERSPECTIVE_RH(RE_PI_F / 2, 1.0f, 0.1f, 100.f);
    test_result("M4F32 perspective",
        approx_eq_f32(P.m[0], 1.0f, 1e-4f) &&
        approx_eq_f32(P.m[5], 1.0f, 1e-4f));
}

static void test_m4_ortho(void)
{
    RE_M4_F32 O = RE_M4F32_ORTHO_RH(-1,1,-1,1,0.1f,10.f);

    test_result("M4F32 ortho scale",
        approx_eq_f32(O.m[0],1.0f,1e-6f) &&
        approx_eq_f32(O.m[5],1.0f,1e-6f));
}

/* ============================================================================================
   SIMD TESTS — Compare SIMD to scalar fallback
   ============================================================================================ */

static void test_m4_simd(void)
{
    RE_M4_F32 A = RE_M4F32_MAKE(
        1,0,0,0,
        2,1,0,0,
        3,4,1,0,
        5,6,7,1
    );
    RE_M4_F32 B = RE_M4F32_MAKE(
        2,3,5,1,
        0,1,4,2,
        7,8,1,3,
        6,9,0,4
    );

    /* Scalar reference */
    RE_M4_F32 REF = RE_M4F32_MULTIPLY(&A,&B);

    /* SIMD result */
    RE_M4_F32 SIMD;
    RE_M4F32_MUL(SIMD.m, A.m, B.m);

    test_result("M4F32 SIMD multiply matches scalar",
        mat4_eq_f32(&REF,&SIMD,1e-5f));
}

/* ============================================================================================
   MASTER RUNNER
   ============================================================================================ */

extern "C" void run_mat_tests(void)
{
    printf("=== RE_MAT tests begin ===\n");

    test_m2();
    test_m3();
    test_m4_basic();
    test_m4_mul();
    test_m4_inverse();
    test_m4_det();
    test_m4_trs();
    test_m4_lookat();
    test_m4_perspective();
    test_m4_ortho();
    test_m4_simd();

    printf("=== RE_MAT tests finished ===\n");
}
