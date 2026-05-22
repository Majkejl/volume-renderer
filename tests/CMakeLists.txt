add_executable(vr_tests test_smoke.cpp)
target_link_libraries(vr_tests PRIVATE GTest::gtest GTest::gtest_main)
vr_set_warnings(vr_tests)

include(GoogleTest)
gtest_discover_tests(vr_tests)
