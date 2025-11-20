cmake_minimum_required(VERSION 3.28)

project(REMath C CXX)

# =============================
# Enable testing
# =============================
enable_testing()

# =============================
# Collect all test .cpp files
# =============================
file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
)

# =============================
# Create test executable
# =============================
add_executable(re_tests
    ${TEST_SOURCES}
)

# =============================
# Include paths
# =============================
target_include_directories(re_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include             # RE Library
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLM    # GLM Library
)

# =============================
# Compile features
# =============================
target_compile_features(re_tests PRIVATE
    c_std_99
    cxx_std_20
)

# =============================
# Register CTest test
# =============================
add_test(NAME re_tests COMMAND re_tests)
