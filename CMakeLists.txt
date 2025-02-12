cmake_minimum_required(VERSION 3.10)
project(lattice-zkp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Download and build Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1 # Use a specific version of Google Test
)
# Ensure Google Test is available
FetchContent_MakeAvailable(googletest)

# Download and build Google Benchmark
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.2 # Use a specific version of Google Benchmark
)
# Ensure Google Benchmark is available
FetchContent_GetProperties(benchmark)
if(NOT benchmark_POPULATED)
    FetchContent_Populate(benchmark)
    # Manually configure Google Benchmark to use the already downloaded Google Test
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable Google Benchmark tests" FORCE)
    add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

# Main library
add_library(lattice-zkp
    src/lwe_utils.cpp
    src/prover.cpp
    src/verifier.cpp
)

# Tests
add_executable(zkp_tests
    test/test_zkp.cpp
    test/test_cases.cpp
)
target_link_libraries(zkp_tests
    PRIVATE
    lattice-zkp
    GTest::gtest
    GTest::gtest_main
)

# Benchmarks
add_executable(benchmark_zkp
    test/benchmark_zkp.cpp
)
target_link_libraries(benchmark_zkp
    PRIVATE
    lattice-zkp
    benchmark::benchmark
)

# Enable testing
enable_testing()
add_test(NAME zkp_tests COMMAND zkp_tests)