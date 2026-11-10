file(GLOB BENCH_SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(bench ${BENCH_SOURCES})
target_link_libraries(bench PRIVATE matrix::matrix)
