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

int main(void)
{
    printf("=== REMath combined test suite start ===\n");

    run_core_tests();
    run_math_tests();
    run_math_ext_tests();

    printf("=== REMath combined test suite finished ===\n");
    return 0;
}
