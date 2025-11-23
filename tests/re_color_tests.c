#include "../include/re_test_core.h"
#include "../include/re_color.h"

#include <stdio.h>

/* ============================================================================================
   TEST HELPERS
   ============================================================================================ */

/**
 * @brief Approximate float comparison helper.
 */
static RE_BOOL approx_eq_f32(RE_f32 a, RE_f32 b, RE_f32 eps)
{
    RE_f32 d = a - b;
    if (d < 0) d = -d;
    return d <= eps;
}


static RE_BOOL approx3(RE_COLORRGBf a, RE_COLORRGBf b, RE_f32 eps)
{
    return approx_eq_f32(a.r, b.r, eps) &&
           approx_eq_f32(a.g, b.g, eps) &&
           approx_eq_f32(a.b, b.b, eps);
}

static RE_BOOL approx4(RE_COLORRGBAf a, RE_COLORRGBAf b, RE_f32 eps)
{
    return approx_eq_f32(a.r, b.r, eps) &&
           approx_eq_f32(a.g, b.g, eps) &&
           approx_eq_f32(a.b, b.b, eps) &&
           approx_eq_f32(a.a, b.a, eps);
}

/* ===============================================================================================
    TEST 1 — BASIC CONSTRUCTORS
   =============================================================================================== */
static void test_color_constructors(void)
{
    RE_COLORRGB8 c1 = RE_COLORRGB8_MAKE(10,20,30);
    test_result("RGB8 ctor r", c1.r == 10);
    test_result("RGB8 ctor g", c1.g == 20);
    test_result("RGB8 ctor b", c1.b == 30);

    RE_COLORRGBA8 c2 = RE_COLORRGBA8_MAKE(1,2,3,4);
    test_result("RGBA8 ctor r", c2.r == 1);
    test_result("RGBA8 ctor g", c2.g == 2);
    test_result("RGBA8 ctor b", c2.b == 3);
    test_result("RGBA8 ctor a", c2.a == 4);
}

/* ===============================================================================================
    TEST 2 — u8 → f32 conversion
   =============================================================================================== */
static void test_color_u8_to_f32(void)
{
    RE_COLORRGB8 u = { 255, 128, 0 };
    RE_COLORRGBf f = RE_COLOR_TO_F32(u);

    test_result("u8->f32 r", approx_eq_f32(f.r, 1.0f, 1e-6f));
    test_result("u8->f32 g", approx_eq_f32(f.g, 128.0f/255.0f, 1e-6f));
    test_result("u8->f32 b", approx_eq_f32(f.b, 0.0f, 1e-6f));
}

static void test_color_u8a_to_f32a(void)
{
    RE_COLORRGBA8 u = { 10, 20, 30, 40 };
    RE_COLORRGBAf f = RE_COLOR_TO_F32A(u);

    test_result("u8a->f32a r", approx_eq_f32(f.r, 10.0f/255.0f, 1e-6f));
    test_result("u8a->f32a g", approx_eq_f32(f.g, 20.0f/255.0f, 1e-6f));
    test_result("u8a->f32a b", approx_eq_f32(f.b, 30.0f/255.0f, 1e-6f));
    test_result("u8a->f32a a", approx_eq_f32(f.a, 40.0f/255.0f, 1e-6f));
}

/* ===============================================================================================
    TEST 3 — f32 → u8 conversion
   =============================================================================================== */
static void test_color_f32_to_u8(void)
{
    RE_COLORRGBf f = { 1.0f, 0.5f, 0.0f };
    RE_COLORRGB8 u = RE_COLOR_TO_u8(f);

    test_result("f32->u8 r", u.r == 255);
    test_result("f32->u8 g", u.g == 127 || u.g == 128);
    test_result("f32->u8 b", u.b == 0);
}

static void test_color_f32a_to_u8a(void)
{
    RE_COLORRGBAf f = { 0.2f, 0.4f, 0.6f, 1.0f };
    RE_COLORRGBA8 u = RE_COLOR_TO_u8A(f);

    test_result("f32a->u8a r", u.r == (RE_u8)(0.2f * 255));
    test_result("f32a->u8a g", u.g == (RE_u8)(0.4f * 255));
    test_result("f32a->u8a b", u.b == (RE_u8)(0.6f * 255));
    test_result("f32a->u8a a", u.a == 255);
}

/* ===============================================================================================
    TEST 4 — HEX ENCODE / DECODE
   =============================================================================================== */
static void test_color_hex(void)
{
    RE_COLORRGBA8 c = { 10, 20, 30, 40 };
    RE_COLORHEX h = RE_COLOR_TO_HEX(c);

    RE_COLORRGBA8 back = RE_COLOR_FROM_HEX(h);

    test_result("HEX r", back.r == 10);
    test_result("HEX g", back.g == 20);
    test_result("HEX b", back.b == 30);
    test_result("HEX a", back.a == 40);
}

/* ===============================================================================================
    TEST 5 — ADD / MUL / LERP
   =============================================================================================== */
static void test_color_add_mul_lerp(void)
{
    RE_COLORRGBf a = { 0.2f, 0.4f, 0.6f };
    RE_COLORRGBf b = { 0.1f, 0.1f, 0.1f };

    RE_COLORRGBf add = RE_COLOR_RGBF_ADD(a,b);
    test_result("RGB add r", approx_eq_f32(add.r, 0.3f, 1e-6f));
    test_result("RGB add g", approx_eq_f32(add.g, 0.5f, 1e-6f));
    test_result("RGB add b", approx_eq_f32(add.b, 0.7f, 1e-6f));

    RE_COLORRGBf mul = RE_COLOR_RGBF_MUL(a,b);
    test_result("RGB mul r", approx_eq_f32(mul.r, 0.02f, 1e-6f));
    test_result("RGB mul g", approx_eq_f32(mul.g, 0.04f, 1e-6f));
    test_result("RGB mul b", approx_eq_f32(mul.b, 0.06f, 1e-6f));

    RE_COLORRGBf l = RE_COLOR_RGBF_LERP(a,b,0.5f);
    test_result("RGB lerp r", approx_eq_f32(l.r, 0.15f, 1e-6f));
    test_result("RGB lerp g", approx_eq_f32(l.g, 0.25f, 1e-6f));
    test_result("RGB lerp b", approx_eq_f32(l.b, 0.35f, 1e-6f));
}

/* ===============================================================================================
    TEST 6 — BRIGHTNESS
   =============================================================================================== */
static void test_color_brightness(void)
{
    RE_COLORRGBf c = { 0.5f, 0.5f, 0.5f };
    RE_COLORRGBf o = RE_COLOR_RGBF_BRIGHTNESS(c,2.0f);

    test_result("brightness r", approx_eq_f32(o.r,1.0f,1e-6f));
    test_result("brightness g", approx_eq_f32(o.g,1.0f,1e-6f));
    test_result("brightness b", approx_eq_f32(o.b,1.0f,1e-6f));
}

/* ===============================================================================================
    TEST 7 — GAMMA
   =============================================================================================== */
static void test_color_gamma(void)
{
    RE_COLORRGBf c = { 0.25f, 0.5f, 0.75f };
    RE_COLORRGBf g = RE_COLOR_RGBF_GAMMA(c,2.0f);

    test_result("gamma r", approx_eq_f32(g.r, 0.25f*0.25f, 1e-5f));
    test_result("gamma g", approx_eq_f32(g.g, 0.25f,       1e-5f));
    test_result("gamma b", approx_eq_f32(g.b, 0.75f*0.75f, 1e-5f));
}

/* ===============================================================================================
    TEST 8 — CONTRAST
   =============================================================================================== */
static void test_color_contrast(void)
{
    RE_COLORRGBf c = {0.5f, 0.5f, 0.5f};
    RE_COLORRGBf o = RE_COLOR_RGBF_CONTRAST(c, 2.0f);

    test_result("contrast gray", approx_eq_f32(o.r,0.5f,1e-6f));
    test_result("contrast gray g", approx_eq_f32(o.g,0.5f,1e-6f));
    test_result("contrast gray b", approx_eq_f32(o.b,0.5f,1e-6f));
}

/* ===============================================================================================
    TEST 9 — EXPOSURE
   =============================================================================================== */
static void test_color_exposure(void)
{
    RE_COLORRGBf c = { 1.0f, 0.5f, 0.0f };
    RE_COLORRGBf o = RE_COLOR_RGBF_EXPOSURE(c,1.0f);

    test_result("exposure r", o.r > 0.63f && o.r < 0.65f);
    test_result("exposure g", o.g > 0.39f && o.g < 0.41f);
    test_result("exposure b", approx_eq_f32(o.b,0.0f,1e-6f));
}

/* ===============================================================================================
    TEST 10 — HSV → RGB
   =============================================================================================== */
static void test_hsv_to_rgb(void)
{
    RE_COLORHSVf h = { 0,1,1 };
    RE_COLORRGBAf r = RE_HSV_TO_RGB_f32(h);

    test_result("HSV red r", approx_eq_f32(r.r,1.0f,1e-6f));
    test_result("HSV red g", approx_eq_f32(r.g,0.0f,1e-6f));
    test_result("HSV red b", approx_eq_f32(r.b,0.0f,1e-6f));
}

/* ===============================================================================================
    TEST 11 — RGB → HSV → RGB roundtrip
   =============================================================================================== */
static void test_hsv_roundtrip(void)
{
    RE_COLORRGBf a = {0.2f,0.6f,0.4f};

    RE_COLORHSVf h = RE_RGB_TO_HSV_f(a);
    RE_COLORRGBAf b = RE_HSV_TO_RGB_f32(h);

    RE_COLORRGBf bf = { b.r, b.g, b.b };

    test_result("HSV roundtrip", approx3(a,bf,2e-2f));
}

/* ===============================================================================================
    TEST 12 — HSL roundtrip
   =============================================================================================== */
static void test_hsl_roundtrip(void)
{
    RE_COLORRGBf a = {0.8f,0.2f,0.6f};

    RE_COLORHSLf h = RE_RGB_TO_HSL_f(a);
    RE_COLORRGBf b = RE_HSL_TO_RGB_f(h);

    test_result("HSL roundtrip", approx3(a,b,1e-2f));
}

/* ===============================================================================================
    MASTER CALLER
   =============================================================================================== */
void test_color_all(void)
{
    test_color_constructors();
    test_color_u8_to_f32();
    test_color_u8a_to_f32a();
    test_color_f32_to_u8();
    test_color_f32a_to_u8a();
    test_color_hex();
    test_color_add_mul_lerp();
    test_color_brightness();
    test_color_gamma();
    test_color_contrast();
    test_color_exposure();
    test_hsv_to_rgb();
    test_hsv_roundtrip();
    test_hsl_roundtrip();
}
