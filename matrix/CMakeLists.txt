find_package(OpenMP REQUIRED)

# New .cpp (and .cu, with CUDA on) files in this folder are picked up automatically
file(GLOB MATRIX_SOURCES CONFIGURE_DEPENDS *.cpp)
if(MATRIX_USE_CUDA)
    file(GLOB MATRIX_CUDA_SOURCES CONFIGURE_DEPENDS *.cu)
    list(APPEND MATRIX_SOURCES ${MATRIX_CUDA_SOURCES})
endif()

add_library(matrix STATIC ${MATRIX_SOURCES} "Cuda.h")
add_library(matrix::matrix ALIAS matrix)

target_include_directories(matrix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(matrix PUBLIC OpenMP::OpenMP_CXX)

if(MATRIX_USE_CUDA)
    find_package(CUDAToolkit REQUIRED)
    # PUBLIC so tests and bench can #ifdef MATRIX_HAS_CUDA to reach the GPU paths
    target_compile_definitions(matrix PUBLIC MATRIX_HAS_CUDA)
    target_link_libraries(matrix PUBLIC CUDA::cudart)
endif()
