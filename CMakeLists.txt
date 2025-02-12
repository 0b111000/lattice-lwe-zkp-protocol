cmake_minimum_required(VERSION 3.10)
project(lwe_zkp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Create library
add_library(lwe_zkp
    src/lwe_utils.cpp
    src/prover.cpp
    src/verifier.cpp
)

# Create example executable
add_executable(zkp_example examples/main.cpp)
target_link_libraries(zkp_example lwe_zkp)

# Add compiler flags
target_compile_options(lwe_zkp PRIVATE -Wall -Wextra)
target_compile_options(zkp_example PRIVATE -Wall -Wextra)