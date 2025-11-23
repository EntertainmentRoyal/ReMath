#include "re_core.h"
#include "re_math.h"

typedef struct {
    RE_u8 r, g, b;
} RE_COLORRGB8;

typedef struct {
    RE_u8 r, g, b, a;
} RE_COLORRGBA8;

typedef struct {
    RE_f32 r, g, b;
} RE_COLORRGBf;

typedef struct {
    RE_f32 r, g, b, a;
} RE_COLORRGBAf;

typedef struct {
    RE_f32 h, s, v;
} RE_COLORHSV;

typedef struct {
    RE_f32 h, s, l;
} RE_COLORHSL;

typedef struct {
    RE_u32 value;
} RE_COLORHEX;

typedef struct RE_COLORHSVf {
    RE_f32 h;   // 0–360 degrees
    RE_f32 s;   // 0–1
    RE_f32 v;   // 0–1
} RE_COLORHSVf;

typedef struct RE_COLORHSLf {
    RE_f32 h; // 0–360 degrees
    RE_f32 s; // 0–1
    RE_f32 l; // 0–1
} RE_COLORHSLf;

 /* =============================================================
       COLOR CONSTRUCTORS
       ============================================================= */

    RE_INLINE RE_COLORRGB8 RE_COLORRGB8_MAKE(RE_u8 r, RE_u8 g, RE_u8 b)
    {
        RE_COLORRGB8 c = { r, g, b };
        return c;
    }

    RE_INLINE RE_COLORRGBA8 RE_COLORRGBA8_MAKE(RE_u8 r, RE_u8 g, RE_u8 b, RE_u8 a)
    {
        RE_COLORRGBA8 c = { r, g, b, a };
        return c;
    }

    RE_INLINE RE_COLORRGBf RE_COLORRGBf_MAKE(RE_f32 r, RE_f32 g, RE_f32 b) {
        RE_COLORRGBf c = { r, g, b };
        return c;
    }

    RE_INLINE RE_COLORRGBAf RE_COLORRGBAf_MAKE(RE_f32 r, RE_f32 g, RE_f32 b, RE_f32 a) {
        RE_COLORRGBAf c = { r, g, b, a };
        return c;
    }

    RE_INLINE RE_COLORRGBf RE_COLOR_TO_F32(RE_COLORRGB8 c)
    {
        return (RE_COLORRGBf) {
            c.r * (1.0f / 255.0f),
            c.g * (1.0f / 255.0f),
            c.b * (1.0f / 255.0f)
        };
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_TO_F32A(RE_COLORRGBA8 c) {
        return (RE_COLORRGBAf) {
            c.r * (1.0f / 255.0f),
            c.g * (1.0f / 255.0f),
            c.b * (1.0f / 255.0f),
            c.a * (1.0f / 255.0f)
        };
    }

    RE_INLINE RE_COLORRGB8 RE_COLOR_TO_u8(RE_COLORRGBf c) {
        return (RE_COLORRGB8) {
            (RE_u8)(RE_CLAMP01(c.r) * 255.0f),
            (RE_u8)(RE_CLAMP01(c.g) * 255.0f),
            (RE_u8)(RE_CLAMP01(c.b) * 255.0f)
        };
    }

    RE_INLINE RE_COLORRGBA8 RE_COLOR_TO_u8A(RE_COLORRGBAf c) {
        return (RE_COLORRGBA8) {
            (RE_u8)(RE_CLAMP01(c.r) * 255.0f),
            (RE_u8)(RE_CLAMP01(c.g) * 255.0f),
            (RE_u8)(RE_CLAMP01(c.b) * 255.0f),
            (RE_u8)(RE_CLAMP01(c.a) * 255.0f)
        };
    }

    RE_INLINE RE_COLORHEX RE_COLOR_TO_HEX(RE_COLORRGBA8 c) {
        RE_COLORHEX h;
        h.value =
            ((RE_u32)c.a << 24) |
            ((RE_u32)c.r << 16) |
            ((RE_u32)c.g <<  8) |
            ((RE_u32)c.b      );
        return h;
    }

    RE_INLINE RE_COLORRGBA8 RE_COLOR_FROM_HEX(RE_COLORHEX h) {
        RE_u32 v = h.value;

        return (RE_COLORRGBA8){
            (RE_u8)((v >> 16) & 0xFF),   // R
            (RE_u8)((v >>  8) & 0xFF),   // G
            (RE_u8)((v      ) & 0xFF),   // B
            (RE_u8)((v >> 24) & 0xFF)    // A
        };
    }

    RE_INLINE RE_COLORRGBf RE_COLOR_RGBF_BRIGHTNESS(RE_COLORRGBf c, RE_f32 b)
    {
        RE_COLORRGBf o;
        o.r = c.r * b;
        o.g = c.g * b;
        o.b = c.b * b;
        return o;
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_RGBAF_BRIGHTNESS(RE_COLORRGBAf c, RE_f32 b)
    {
        RE_COLORRGBAf o;
        o.r = c.r * b;
        o.g = c.g * b;
        o.b = c.b * b;
        o.a = c.a;
        return o;
    }

    RE_INLINE RE_COLORRGBf RE_COLOR_RGBF_ADD(RE_COLORRGBf a, RE_COLORRGBf b)
    {
        RE_COLORRGBf o = { a.r + b.r, a.g + b.g, a.b + b.b };
        return o;
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_RGBAF_ADD(RE_COLORRGBAf a, RE_COLORRGBAf b)
    {
        RE_COLORRGBAf o = { a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a };
        return o;
    }

    RE_INLINE RE_COLORRGBf  RE_COLOR_RGBF_MUL(RE_COLORRGBf a, RE_COLORRGBf b)
    {
        RE_COLORRGBf o = { a.r * b.r, a.g * b.g, a.b * b.b };
        return o;
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_RGBAF_MUL(RE_COLORRGBAf a, RE_COLORRGBAf b)
    {
        RE_COLORRGBAf o = { a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a };
        return o;
    }

    RE_INLINE RE_COLORRGBf RE_COLOR_RGBF_LERP(RE_COLORRGBf a, RE_COLORRGBf b, RE_f32 t)
    {
        RE_COLORRGBf o;
        o.r = a.r + t * (b.r - a.r);
        o.g = a.g + t * (b.g - a.g);
        o.b = a.b + t * (b.b - a.b);
        return o;
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_RGBAF_LERP(RE_COLORRGBAf a, RE_COLORRGBAf b, RE_f32 t)
    {
        RE_COLORRGBAf o;
        o.r = a.r + t * (b.r - a.r);
        o.g = a.g + t * (b.g - a.g);
        o.b = a.b + t * (b.b - a.b);
        o.a = a.a + t * (b.a - a.a);
        return o;
    }

    RE_INLINE RE_COLORRGBf RE_COLOR_RGBF_GAMMA(RE_COLORRGBf c, RE_f32 g)
    {
        RE_COLORRGBf o;
        o.r = RE_POW_f32(c.r, g);
        o.g = RE_POW_f32(c.g, g);
        o.b = RE_POW_f32(c.b, g);
        return o;
    }

    RE_INLINE RE_COLORRGBf RE_COLOR_RGBF_DEGAMMA(RE_COLORRGBf c, RE_f32 g)
    {
        RE_COLORRGBf o;
        RE_f32 inv = 1.0f / g;
        o.r = RE_POW_f32(c.r, inv);
        o.g = RE_POW_f32(c.g, inv);
        o.b = RE_POW_f32(c.b, inv);
        return o;
    }

    RE_INLINE RE_COLORRGBf RE_COLOR_RGBF_CONTRAST(RE_COLORRGBf c, RE_f32 k)
    {
        RE_COLORRGBf o;
        o.r = (c.r - 0.5f) * k + 0.5f;
        o.g = (c.g - 0.5f) * k + 0.5f;
        o.b = (c.b - 0.5f) * k + 0.5f;
        return o;
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_RGBAF_CONTRAST(RE_COLORRGBAf c, RE_f32 k)
    {
        RE_COLORRGBAf o;
        o.r = (c.r - 0.5f) * k + 0.5f;
        o.g = (c.g - 0.5f) * k + 0.5f;
        o.b = (c.b - 0.5f) * k + 0.5f;
        o.a = c.a;
        return o;
    }

    RE_INLINE RE_COLORRGBf RE_COLOR_RGBF_EXPOSURE(RE_COLORRGBf c, RE_f32 e)
    {
        RE_COLORRGBf o;
        o.r = 1.0f - RE_EXP_f32(-c.r * e);
        o.g = 1.0f - RE_EXP_f32(-c.g * e);
        o.b = 1.0f - RE_EXP_f32(-c.b * e);
        return o;
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_BRIGHTNESS(RE_COLORRGBAf c, RE_f32 b)
    {
        RE_COLORRGBAf o;
        o.r = RE_CLAMP01(c.r + b);
        o.g = RE_CLAMP01(c.g + b);
        o.b = RE_CLAMP01(c.b + b);
        o.a = c.a;
        return o;
    }

    RE_INLINE RE_COLORRGBA8 RE_COLOR_BRIGHTNESS8(RE_COLORRGBA8 c, RE_i32 b)
    {
        RE_COLORRGBA8 o;
        o.r = (RE_u8)RE_CLAMP((int)c.r + b, 0, 255);
        o.g = (RE_u8)RE_CLAMP((int)c.g + b, 0, 255);
        o.b = (RE_u8)RE_CLAMP((int)c.b + b, 0, 255);
        o.a = c.a;
        return o;
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_MUL(RE_COLORRGBAf a, RE_COLORRGBAf b)
    {
        return (RE_COLORRGBAf){
            a.r * b.r,
            a.g * b.g,
            a.b * b.b,
            a.a * b.a
        };
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_MULS(RE_COLORRGBAf a, RE_f32 s)
    {
        return (RE_COLORRGBAf){
            a.r * s,
            a.g * s,
            a.b * s,
            a.a
        };
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_ADD(RE_COLORRGBAf a, RE_COLORRGBAf b)
    {
        return (RE_COLORRGBAf){
            RE_CLAMP01(a.r + b.r),
            RE_CLAMP01(a.g + b.g),
            RE_CLAMP01(a.b + b.b),
            RE_CLAMP01(a.a + b.a)
        };
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_LERP(RE_COLORRGBAf a, RE_COLORRGBAf b, RE_f32 t)
    {
        return (RE_COLORRGBAf){
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t
        };
    }
    RE_INLINE RE_COLORRGBAf RE_COLOR_GAMMA(RE_COLORRGBAf c, RE_f32 g)
    {
        return (RE_COLORRGBAf){
            RE_POW_f32(c.r, g),
            RE_POW_f32(c.g, g),
            RE_POW_f32(c.b, g),
            c.a
        };
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_INGAMMA(RE_COLORRGBAf c, RE_f32 g)
    {
        RE_f32 inv = 1.0f / g;
        return (RE_COLORRGBAf){
            RE_POW_f32(c.r, inv),
            RE_POW_f32(c.g, inv),
            RE_POW_f32(c.b, inv),
            c.a
        };
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_CONTRAST(RE_COLORRGBAf c, RE_f32 k)
    {
        return (RE_COLORRGBAf){
            RE_CLAMP01((c.r - 0.5f) * k + 0.5f),
            RE_CLAMP01((c.g - 0.5f) * k + 0.5f),
            RE_CLAMP01((c.b - 0.5f) * k + 0.5f),
            c.a
        };
    }

    RE_INLINE RE_COLORRGBAf RE_COLOR_EXPOSURE(RE_COLORRGBAf c, RE_f32 e)
    {
        return (RE_COLORRGBAf){
            1.0f - RE_EXP_f32(-c.r * e),
            1.0f - RE_EXP_f32(-c.g * e),
            1.0f - RE_EXP_f32(-c.b * e),
            c.a
        };
    }

        RE_INLINE RE_COLORRGBAf RE_HSV_TO_RGB_f32(RE_COLORHSVf h)
        {
            RE_f32 C = h.v * h.s;
            RE_f32 X = C * (1.0f - RE_ABS(RE_FMOD_f32(h.h / 60.0f, 2.0f) - 1.0f));
            RE_f32 m = h.v - C;

            RE_f32 r1, g1, b1;

            if      (h.h < 60.0f)  { r1 = C; g1 = X; b1 = 0; }
            else if (h.h < 120.0f) { r1 = X; g1 = C; b1 = 0; }
            else if (h.h < 180.0f) { r1 = 0; g1 = C; b1 = X; }
            else if (h.h < 240.0f) { r1 = 0; g1 = X; b1 = C; }
            else if (h.h < 300.0f) { r1 = X; g1 = 0; b1 = C; }
            else                   { r1 = C; g1 = 0; b1 = X; }

            return (RE_COLORRGBAf){
                r1 + m,
                g1 + m,
                b1 + m,
                1.0f
            };
        }

    RE_INLINE RE_COLORHSVf RE_RGB_TO_HSV_f(RE_COLORRGBf c)
    {
        RE_COLORHSVf out;

        RE_f32 maxv = RE_FMAX_f32(c.r, RE_FMAX_f32(c.g, c.b));
        RE_f32 minv = RE_FMIN_f32(c.r, RE_FMIN_f32(c.g, c.b));
        RE_f32 delta = maxv - minv;

        // Hue
        if (delta < 1e-6f) {
            out.h = 0.0f;
        }
        else if (maxv == c.r) {
            out.h = 60.0f * RE_FMOD_f32(((c.g - c.b) / delta), 6.0f);
        }
        else if (maxv == c.g) {
            out.h = 60.0f * (((c.b - c.r) / delta) + 2.0f);
        }
        else {
            out.h = 60.0f * (((c.r - c.g) / delta) + 4.0f);
        }

        if (out.h < 0.0f)
            out.h += 360.0f;

        out.s = (maxv <= 0.0f) ? 0.0f : (delta / maxv);

        out.v = maxv;

        return out;
    }

    RE_INLINE RE_COLORHSLf RE_RGB_TO_HSL_f(RE_COLORRGBf c)
    {
        RE_COLORHSLf out;

        RE_f32 maxv = RE_FMAX_f32(c.r, RE_FMAX_f32(c.g, c.b));
        RE_f32 minv = RE_FMIN_f32(c.r, RE_FMIN_f32(c.g, c.b));
        RE_f32 delta = maxv - minv;

        // Lightness
        RE_f32 l = (maxv + minv) * 0.5f;

        RE_f32 s = 0.0f;
        RE_f32 h = 0.0f;

        if (delta > 1e-6f) {
            if (l < 0.5f)
                s = delta / (maxv + minv);
            else
                s = delta / (2.0f - maxv - minv);

            if (maxv == c.r)
                h = 60.0f * RE_FMOD_f32(((c.g - c.b) / delta), 6.0f);
            else if (maxv == c.g)
                h = 60.0f * (((c.b - c.r) / delta) + 2.0f);
            else
                h = 60.0f * (((c.r - c.g) / delta) + 4.0f);
        }

        if (h < 0.0f)
            h += 360.0f;

        out.h = h;
        out.s = s;
        out.l = l;

        return out;
    }

    RE_INLINE RE_f32 RE_HUE_TO_RGB(RE_f32 p, RE_f32 q, RE_f32 t)
    {
        if (t < 0.0f) t += 1.0f;
        if (t > 1.0f) t -= 1.0f;

        if (t < 1.0f/6.0f) return p + (q - p) * 6.0f * t;
        if (t < 1.0f/2.0f) return q;
        if (t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6.0f;

        return p;
    }


    RE_INLINE RE_COLORRGBf RE_HSL_TO_RGB_f(RE_COLORHSLf hsl)
    {
        RE_COLORRGBf out;

        RE_f32 h = hsl.h / 360.0f;
        RE_f32 s = RE_CLAMP01(hsl.s);
        RE_f32 l = RE_CLAMP01(hsl.l);

        if (s <= 0.0f) {
            out.r = out.g = out.b = l;
            return out;
        }

        RE_f32 q = (l < 0.5f)
                 ? (l * (1.0f + s))
                 : (l + s - l * s);

        RE_f32 p = 2.0f * l - q;

        out.r = RE_HUE_TO_RGB(p, q, h + 1.0f/3.0f);
        out.g = RE_HUE_TO_RGB(p, q, h);
        out.b = RE_HUE_TO_RGB(p, q, h - 1.0f/3.0f);

        return out;
    }
