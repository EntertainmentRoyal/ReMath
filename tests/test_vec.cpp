/**
 * @file test_vec.cpp
 * @brief Unit tests for typed Vec3 (RE_V3_f32) using GLM for reference validation.
 *
 * Updated to match NEW API naming:
 *
 *   RE_V3_f32                → vector type
 *   RE_V3_MAKE_f32(x,y,z)    → constructor
 *   RE_V3_ADD_f32(a,b)       → add
 *   RE_V3_SUB_f32(a,b)       → sub
 *   RE_V3_DOT_f32(a,b)       → dot
 *   RE_V3_HADAMARD_f32       → element multiply
 *   RE_V3_CLAMP_f32          → clamp
 *   RE_V3_LERP_f32           → lerp
 *
 *   Float-only advanced:
 *      RE_V3_LENGTH_f32
 *      RE_V3_NORMALIZE_f32
 *      RE_V3_DISTANCE_f32
 *      RE_V3_CROSS_f32
 *      RE_V3_REFLECT_f32
 *      RE_V3_REFRACT_f32
 *      RE_V3_ANGLE_f32
 *      RE_V3_PROJECT_f32
 *
 * Author: Jayansh Devgans
 */

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <cmath>

extern "C" {
#include "../include/re_vec.h"
#include "../include/re_test_core.h"
}

/* Compare floats */
static bool approx(float a, float b, float eps = 1e-4f)
{
    return std::fabs(a - b) <= eps;
}

/* Compare RE_V3_f32 to glm::vec3 */
static bool approx_vec3(const RE_V3_f32& a, const glm::vec3& b, float eps = 1e-4f)
{
    return approx(a.x, b.x, eps) &&
           approx(a.y, b.y, eps) &&
           approx(a.z, b.z, eps);
}

/* ======================================================================================================
   BASIC TESTS (length, normalize, cross, distance)
   ====================================================================================================== */

static void test_vec3_basic()
{
    RE_V3_f32 a = RE_V3_MAKE_f32(1, 2, 3);
    glm::vec3 g(1,2,3);

    /* Length */
    float l1 = RE_V3_LENGTH_f32(a);
    float l2 = glm::length(g);
    test_result("V3_LENGTH_f32", approx(l1, l2));

    /* Normalize */
    RE_V3_f32 n1 = RE_V3_NORMALIZE_f32(a);
    glm::vec3 n2 = glm::normalize(g);
    test_result("V3_NORMALIZE_f32", approx_vec3(n1, n2));

    /* Cross */
    RE_V3_f32 c1 = RE_V3_CROSS_f32(a, RE_V3_MAKE_f32(4,5,6));
    glm::vec3 c2 = glm::cross(g, glm::vec3(4,5,6));
    test_result("V3_CROSS_f32", approx_vec3(c1, c2));

    /* Distance */
    float d1 = RE_V3_DISTANCE_f32(a, RE_V3_MAKE_f32(4,5,6));
    float d2 = glm::distance(g, glm::vec3(4,5,6));
    test_result("V3_DISTANCE_f32", approx(d1, d2));
}

/* ======================================================================================================
   ADVANCED OPS — hadamard, clamp, lerp, reflect, refract
   ====================================================================================================== */

static void test_vec3_ops()
{
    RE_V3_f32 a = RE_V3_MAKE_f32(1,2,3);
    RE_V3_f32 b = RE_V3_MAKE_f32(-4,5,2);

    glm::vec3 g1(1,2,3);
    glm::vec3 g2(-4,5,2);

    /* Hadamard */
    RE_V3_f32 h1 = RE_V3_HADAMARD_f32(a, b);
    glm::vec3 h2 = g1 * g2;
    test_result("V3_HADAMARD_f32", approx_vec3(h1, h2));

    /* Clamp */
    RE_V3_f32 cl = RE_V3_CLAMP_f32(a,
                                   RE_V3_BROADCAST_f32(-1),
                                   RE_V3_BROADCAST_f32(2));
    glm::vec3 gl = glm::clamp(g1, glm::vec3(-1), glm::vec3(2));
    test_result("V3_CLAMP_f32", approx_vec3(cl, gl));

    /* Lerp */
    RE_V3_f32 l1 = RE_V3_LERP_f32(a, b, 0.25f);
    glm::vec3 l2 = glm::mix(g1, g2, 0.25f);
    test_result("V3_LERP_f32", approx_vec3(l1, l2));

    /* Reflect */
    RE_V3_f32 r1 = RE_V3_REFLECT_f32(a, RE_V3_NORMALIZE_f32(b));
    glm::vec3 r2 = glm::reflect(g1, glm::normalize(g2));
    test_result("V3_REFLECT_f32", approx_vec3(r1, r2, 1e-3f));

    /* Refract */
    RE_V3_f32 rf1 = RE_V3_REFRACT_f32(a, RE_V3_NORMALIZE_f32(b), 1.0f);
    glm::vec3 rf2 = glm::refract(g1, glm::normalize(g2), 1.0f);
    test_result("V3_REFRACT_f32", approx_vec3(rf1, rf2, 3e-3f));
}

/* ======================================================================================================
   TESTING
   ====================================================================================================== */

static float frand()
{
    return (float(rand()) / RAND_MAX) * 2.0f - 1.0f;
}

static void test_vec3_fuzz()
{
    for (int i = 0; i < 300; i++)
    {
        RE_V3_f32 a = RE_V3_MAKE_f32(frand(), frand(), frand());
        RE_V3_f32 b = RE_V3_MAKE_f32(frand(), frand(), frand());

        glm::vec3 g1(a.x, a.y, a.z);
        glm::vec3 g2(b.x, b.y, b.z);

        /* Length */
        test_result("LENGTH_f32",
            approx(RE_V3_LENGTH_f32(a), glm::length(g1), 1e-3f));

        /* Normalize */
        if (glm::length(g1) > 1e-5f)
        {
            test_result("NORMALIZE_f32",
                approx_vec3(RE_V3_NORMALIZE_f32(a), glm::normalize(g1), 1e-3f));
        }

        /* Hadamard */
        test_result("HADMARD_f32",
            approx_vec3(RE_V3_HADAMARD_f32(a,b), g1 * g2, 1e-3f));
    }
}

/* ======================================================================================================
   ENTRY
   ====================================================================================================== */

extern "C" void run_vec_tests(void)
{
    std::cout << "=== Vec tests start ===\n";

    test_vec3_basic();
    test_vec3_ops();
    test_vec3_fuzz();

    std::cout << "=== Vec tests finished ===\n";
}
