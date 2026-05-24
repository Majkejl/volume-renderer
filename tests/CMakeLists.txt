# Camera, ShaderSource, VolumeData, and SyntheticVolume are GL-free, so the test
# target compiles them directly and stays context-free (no GL/window needed to
# run the tests). glm is header-only, so linking it adds no GL requirement.
add_executable(vr_tests
    test_smoke.cpp
    test_shader_source.cpp
    test_camera.cpp
    test_volume_data.cpp
    test_synthetic_volume.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/Camera.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/ShaderSource.cpp
    ${CMAKE_SOURCE_DIR}/src/volume/SyntheticVolume.cpp
    ${CMAKE_SOURCE_DIR}/src/volume/VolumeData.cpp)
target_include_directories(vr_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(vr_tests PRIVATE glm::glm GTest::gtest GTest::gtest_main)
vr_set_warnings(vr_tests)

include(GoogleTest)
gtest_discover_tests(vr_tests)
