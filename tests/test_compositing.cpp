#include "render/Compositing.h"

#include <glm/glm.hpp>

#include <gtest/gtest.h>

// Front-to-back compositing accumulation against hand-computed values.

namespace {
constexpr float kEps = 1e-6f;
} // namespace

TEST(CompositingTest, SingleSemiTransparentStep) {
    glm::vec4 dst(0.0f);
    vr::compositeFrontToBack(dst, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
    EXPECT_NEAR(dst.r, 0.5f, kEps);
    EXPECT_NEAR(dst.g, 0.0f, kEps);
    EXPECT_NEAR(dst.b, 0.0f, kEps);
    EXPECT_NEAR(dst.a, 0.5f, kEps);
}

TEST(CompositingTest, TwoStepsAccumulateFrontToBack) {
    glm::vec4 dst(0.0f);
    vr::compositeFrontToBack(dst, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)); // front, red
    vr::compositeFrontToBack(dst, glm::vec4(0.0f, 1.0f, 0.0f, 0.5f)); // behind, green

    // Front contributes 0.5 red; the green behind is attenuated by (1 - 0.5).
    EXPECT_NEAR(dst.r, 0.5f, kEps);
    EXPECT_NEAR(dst.g, 0.25f, kEps);
    EXPECT_NEAR(dst.a, 0.75f, kEps);
}

TEST(CompositingTest, OpaqueFrontOccludesBack) {
    glm::vec4 dst(0.0f);
    vr::compositeFrontToBack(dst, glm::vec4(0.2f, 0.4f, 0.6f, 1.0f)); // fully opaque
    vr::compositeFrontToBack(dst, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // fully hidden

    EXPECT_NEAR(dst.r, 0.2f, kEps);
    EXPECT_NEAR(dst.g, 0.4f, kEps);
    EXPECT_NEAR(dst.b, 0.6f, kEps);
    EXPECT_NEAR(dst.a, 1.0f, kEps);
}
