/**
 * @file re_all_tests.c
 * @brief Master test runner for all REMath test suites.
 *
 * @author
 * Jayansh Devgan
 */

#include <stdio.h>

void run_core_tests(void);
void run_math_tests(void);
void run_math_ext_tests(void);
void run_vec_tests(void);
void run_mat_tests(void);
void run_quat_tests(void);

int main(void)
{
    printf("=== REMath combined test suite start ===\n");

    run_core_tests();
    run_math_tests();
    run_math_ext_tests();
    run_vec_tests();
    run_mat_tests();
    run_quat_tests();

    printf("=== REMath combined test suite finished ===\n");
    return 0;
}
