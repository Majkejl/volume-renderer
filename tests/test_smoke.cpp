#include <gtest/gtest.h>

// Proves the GoogleTest target builds, links, and is discovered/run by CTest.
// Real logic tests (camera math, MRC parsing, ray-box intersection, ...) arrive
// alongside the code that needs them in later milestones.
TEST(SmokeTest, FrameworkRuns) {
    EXPECT_EQ(2 + 2, 4);
}
