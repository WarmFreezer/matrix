file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

# No test sources yet; the target appears once a .cpp is added here
if(TEST_SOURCES)
    add_executable(tests ${TEST_SOURCES})
    target_link_libraries(tests PRIVATE matrix::matrix)
    add_test(NAME matrix_tests COMMAND tests)
endif()
