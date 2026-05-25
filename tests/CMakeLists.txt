# Camera, ShaderSource, VolumeData, and SyntheticVolume are GL-free, so the test
# target compiles them directly and stays context-free (no GL/window needed to
# run the tests). glm is header-only, so linking it adds no GL requirement.
add_executable(vr_tests
    test_smoke.cpp
    test_shader_source.cpp
    test_camera.cpp
    test_volume_data.cpp
    test_synthetic_volume.cpp
    test_ray_box.cpp
    test_compositing.cpp
    test_transfer_function.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/Camera.cpp
    ${CMAKE_SOURCE_DIR}/src/gfx/ShaderSource.cpp
    ${CMAKE_SOURCE_DIR}/src/render/TransferFunction.cpp
    ${CMAKE_SOURCE_DIR}/src/volume/SyntheticVolume.cpp
    ${CMAKE_SOURCE_DIR}/src/volume/VolumeData.cpp)
target_include_directories(vr_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(vr_tests PRIVATE glm::glm GTest::gtest GTest::gtest_main)
vr_set_warnings(vr_tests)

# The slab method intentionally divides by zero-component ray directions to get
# IEEE infinities. With the axis-aligned test inputs this constant-folds to a
# literal 1.0/0.0, which MSVC's optimizer flags as C4723 (potential divide by
# 0) — a false alarm here. Suppress it just for the ray-box test (MSVC only).
if(MSVC)
    set_source_files_properties(test_ray_box.cpp PROPERTIES COMPILE_OPTIONS /wd4723)
endif()

include(GoogleTest)
gtest_discover_tests(vr_tests)
