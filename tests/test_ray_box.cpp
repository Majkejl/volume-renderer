#include "render/RayBox.h"

#include <glm/glm.hpp>

#include <gtest/gtest.h>

// Canonical ray–box cases against the unit box [0,1]^3. This validates the slab
// algorithm mirrored in shaders/raycast.frag.

namespace {
const glm::vec3 kBoxMin(0.0f);
const glm::vec3 kBoxMax(1.0f);
constexpr float kEps = 1e-5f;
} // namespace

TEST(RayBoxTest, ThroughCenter) {
    const vr::RayBoxHit hit = vr::intersectRayBox(glm::vec3(0.5f, 0.5f, -1.0f),
                                                  glm::vec3(0.0f, 0.0f, 1.0f), kBoxMin, kBoxMax);
    EXPECT_TRUE(hit.hit);
    EXPECT_NEAR(hit.tNear, 1.0f, kEps); // enters the z=0 face
    EXPECT_NEAR(hit.tFar, 2.0f, kEps);  // exits the z=1 face
}

TEST(RayBoxTest, DiagonalThroughCorners) {
    const glm::vec3 origin(-1.0f);
    const glm::vec3 dir = glm::normalize(glm::vec3(1.0f));
    const vr::RayBoxHit hit = vr::intersectRayBox(origin, dir, kBoxMin, kBoxMax);

    EXPECT_TRUE(hit.hit);
    const glm::vec3 entry = origin + hit.tNear * dir;
    const glm::vec3 exit = origin + hit.tFar * dir;
    EXPECT_NEAR(glm::length(entry - glm::vec3(0.0f)), 0.0f, 1e-4f); // corner (0,0,0)
    EXPECT_NEAR(glm::length(exit - glm::vec3(1.0f)), 0.0f, 1e-4f);  // corner (1,1,1)
}

TEST(RayBoxTest, OriginInsideStartsAtZero) {
    const vr::RayBoxHit hit =
        vr::intersectRayBox(glm::vec3(0.5f), glm::vec3(0.0f, 0.0f, 1.0f), kBoxMin, kBoxMax);
    EXPECT_TRUE(hit.hit);
    EXPECT_NEAR(hit.tNear, 0.0f, kEps); // clamped, so it marches from the origin
    EXPECT_NEAR(hit.tFar, 0.5f, kEps);
}

TEST(RayBoxTest, CompleteMiss) {
    const vr::RayBoxHit hit = vr::intersectRayBox(glm::vec3(2.0f, 2.0f, -1.0f),
                                                  glm::vec3(0.0f, 0.0f, 1.0f), kBoxMin, kBoxMax);
    EXPECT_FALSE(hit.hit);
}

TEST(RayBoxTest, BoxBehindOrigin) {
    const vr::RayBoxHit hit = vr::intersectRayBox(glm::vec3(0.5f, 0.5f, 5.0f),
                                                  glm::vec3(0.0f, 0.0f, 1.0f), kBoxMin, kBoxMax);
    EXPECT_FALSE(hit.hit); // box lies entirely behind the ray
}
