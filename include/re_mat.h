#ifndef RE_MAT_H
#define RE_MAT_H

/*
    REMath Matrix Library
    Header-only, C99, supports F32 and F64.

    Structure:
        re_mat.h       (umbrella)
        re_mat2.h      (2x2)
        re_mat3.h      (3x3)
        re_mat4.h      (4x4)
        re_mat4_simd.h (SIMD)

    Requires:
        re_core.h  (for RE_f32, RE_f64)
        re_vec.h   (optional, only for convenience)

    Author: Jayansh Devgan
*/

#include "re_core.h"
#include "re_vec.h"

#include "re_mat2.h"
#include "re_mat3.h"
#include "re_mat4.h"
#include "re_mat4_simd.h"

#endif /* RE_MAT_H */
