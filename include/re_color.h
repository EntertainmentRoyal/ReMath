#include "re_core.h"
#include "re_math.h"

namespace REMath
{
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

    /* =============================================================
       COLOR CONSTRUCTORS
       ============================================================= */

    RE_INLINE RE_COLORRGB8 RE_COLORRGB8_MAKE(RE_u8 r, RE_u8 g, RE_u8 b)
    {
        RE_COLORRGB8 c = { r, g, b };
        return c;
    }

    RE_INLINE RE_COLORRGBA8 RE_COLORRGB8_MAKE(RE_u8 r, RE_u8 g, RE_u8 b, RE_u8 a)
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

}
