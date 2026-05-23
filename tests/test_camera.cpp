#include "gfx/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <gtest/gtest.h>

// Camera is pure GLM math, so these tests run with no GL context. They check
// orbit orientation, the elevation clamp, zoom, and that inverseViewProjection
// truly inverts the view-projection (the basis for per-pixel ray casting).

namespace {

constexpr float kEps = 1e-4f;

::testing::AssertionResult vecNear(const glm::vec3& a, const glm::vec3& b, float eps) {
    if (glm::distance(a, b) <= eps) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure() << "(" << a.x << ", " << a.y << ", " << a.z << ") vs ("
                                         << b.x << ", " << b.y << ", " << b.z << ")";
}

} // namespace

TEST(CameraTest, DefaultEyeLooksDownNegativeZ) {
    const vr::Camera camera;
    const glm::vec3 eye = camera.position();
    EXPECT_NEAR(eye.x, 0.0f, kEps);
    EXPECT_NEAR(eye.y, 0.0f, kEps);
    EXPECT_GT(eye.z, 0.0f); // eye sits on +Z, looking back at the origin target
}

TEST(CameraTest, OrbitAzimuthRotatesAroundY) {
    vr::Camera camera;
    const glm::vec3 start = camera.position();

    camera.orbit(glm::half_pi<float>(), 0.0f); // 90 degrees of azimuth
    const glm::vec3 rotated = camera.position();

    EXPECT_NEAR(glm::length(rotated), glm::length(start), kEps); // distance preserved
    EXPECT_NEAR(glm::normalize(rotated).x, 1.0f, kEps);          // now on +X
    EXPECT_NEAR(rotated.y, 0.0f, kEps);
}

TEST(CameraTest, ElevationIsClampedBelowThePole) {
    vr::Camera camera;
    camera.orbit(0.0f, 100.0f); // far past vertical

    const glm::vec3 eye = camera.position();
    EXPECT_GT(eye.y, 0.0f);                    // we moved up
    EXPECT_LT(eye.y, glm::length(eye) - kEps); // but stayed off the pole
}

TEST(CameraTest, ZoomInThenOutIsReversible) {
    vr::Camera camera;
    const float d0 = glm::length(camera.position());

    camera.zoom(1.0f);
    EXPECT_LT(glm::length(camera.position()), d0); // zoom in shrinks distance

    camera.zoom(-1.0f);
    EXPECT_NEAR(glm::length(camera.position()), d0, kEps); // and back again
}

TEST(CameraTest, InverseViewProjectionRoundTrip) {
    vr::Camera camera;
    camera.setAspect(16.0f / 9.0f);
    camera.orbit(0.7f, 0.3f); // arbitrary orientation

    const glm::mat4 viewProj = camera.viewProjectionMatrix();
    const glm::mat4 invViewProj = camera.inverseViewProjection();

    const glm::vec3 worldPoint(0.2f, -0.1f, 0.15f); // inside the frustum
    const glm::vec4 clip = viewProj * glm::vec4(worldPoint, 1.0f);
    const glm::vec3 ndc = glm::vec3(clip) / clip.w;

    const glm::vec4 unprojected = invViewProj * glm::vec4(ndc, 1.0f);
    const glm::vec3 recovered = glm::vec3(unprojected) / unprojected.w;

    EXPECT_TRUE(vecNear(recovered, worldPoint, 1e-3f));
}
