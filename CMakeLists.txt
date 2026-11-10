cmake_minimum_required(VERSION 3.24)
project(matrix VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CUDA is on by default when nvcc is found; -DMATRIX_USE_CUDA=OFF builds the CPU-only library
include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    set(MATRIX_CUDA_DEFAULT ON)
else()
    set(MATRIX_CUDA_DEFAULT OFF)
endif()
option(MATRIX_USE_CUDA "Build the CUDA kernels" ${MATRIX_CUDA_DEFAULT})

if(MATRIX_USE_CUDA)
    # Compile for the GPU in this machine unless the user picks architectures
    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        set(CMAKE_CUDA_ARCHITECTURES native)
    endif()
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 20)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()

# Bench and tests build only when this is the main project, not when pulled in with FetchContent
option(MATRIX_BUILD_BENCH "Build the benchmark" ${PROJECT_IS_TOP_LEVEL})
option(MATRIX_BUILD_TESTS "Build the tests" ${PROJECT_IS_TOP_LEVEL})

add_subdirectory(matrix)

if(MATRIX_BUILD_BENCH)
    add_subdirectory(bench)
endif()

if(MATRIX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
