# ShaderSource.cpp is GL-free, so the test target compiles it directly and stays
# context-free (no GL/window needed to run the tests).
add_executable(vr_tests
    test_smoke.cpp
    test_shader_source.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/ShaderSource.cpp)
target_include_directories(vr_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(vr_tests PRIVATE GTest::gtest GTest::gtest_main)
vr_set_warnings(vr_tests)

include(GoogleTest)
gtest_discover_tests(vr_tests)
