cmake_minimum_required(VERSION 3.28)

project(REMath C)

# ============================================
# Enable testing support
# ============================================
enable_testing()

# ============================================
# Collect ALL test .c files from /tests folder
# ============================================
file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c
)

# ============================================
# Create ONE executable that contains ALL tests
# ============================================
add_executable(re_tests
    ${TEST_SOURCES}
)

target_include_directories(re_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(re_tests PRIVATE c_std_99)
target_link_libraries(re_tests PRIVATE m)

# ============================================
# Register ONE test with CTest
# ============================================
add_test(NAME re_tests COMMAND re_tests)
