#include "./re_constants.h"

#include <stdio.h>
#include <assert.h>

/* ============================================================================================
   TERMINAL COLORS
   ============================================================================================ */

#define COLOR_RED   "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_RESET "\033[0m"


/**
 * @brief Prints PASS/FAIL for each test (used only for dev debugging).
 */
 static void test_result(const char *name, RE_bool success)
 {
     printf("[ %-40s ] %s%s%s\n",
            name,
            success ? COLOR_GREEN : COLOR_RED,
            success ? "PASS" : "FAIL",
            COLOR_RESET);

     /* Hard assertion to stop the suite on any failure */
     assert(success);
 }
