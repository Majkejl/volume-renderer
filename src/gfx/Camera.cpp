#include "gfx/Camera.h"

#include <algorithm>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace vr {

namespace {

constexpr float kDefaultDistance = 2.5f;
constexpr float kMinDistance = 0.2f;
constexpr float kMaxDistance = 50.0f;

// Each scroll step scales distance by this factor; positive steps zoom in.
constexpr float kZoomPerStep = 0.9f;

constexpr float kNearPlane = 0.1f;
constexpr float kFarPlane = 100.0f;
constexpr float kDefaultAspect = 16.0f / 9.0f;

// glm::radians is not reliably constexpr across versions, so use const init.
const float kDefaultFovY = glm::radians(45.0f);
const float kMaxElevation = glm::radians(89.0f); // just shy of the pole

} // namespace

Camera::Camera()
    : target_(0.0f), azimuth_(0.0f), elevation_(0.0f), distance_(kDefaultDistance),
      fovY_(kDefaultFovY), aspect_(kDefaultAspect), nearPlane_(kNearPlane), farPlane_(kFarPlane) {}

void Camera::orbit(float deltaAzimuth, float deltaElevation) {
    azimuth_ += deltaAzimuth;
    elevation_ = std::clamp(elevation_ + deltaElevation, -kMaxElevation, kMaxElevation);
}

void Camera::zoom(float steps) {
    distance_ = std::clamp(distance_ * std::pow(kZoomPerStep, steps), kMinDistance, kMaxDistance);
}

void Camera::setAspect(float aspect) {
    if (aspect > 0.0f) {
        aspect_ = aspect;
    }
}

glm::vec3 Camera::position() const {
    // Spherical coordinates around the target: azimuth sweeps the XZ plane,
    // elevation lifts toward +Y. At (0, 0) the eye sits on +Z looking at -Z.
    const float cosElevation = std::cos(elevation_);
    const glm::vec3 offset(cosElevation * std::sin(azimuth_), std::sin(elevation_),
                           cosElevation * std::cos(azimuth_));
    return target_ + distance_ * offset;
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(position(), target_, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::projectionMatrix() const {
    return glm::perspective(fovY_, aspect_, nearPlane_, farPlane_);
}

glm::mat4 Camera::viewProjectionMatrix() const {
    return projectionMatrix() * viewMatrix();
}

glm::mat4 Camera::inverseViewProjection() const {
    return glm::inverse(viewProjectionMatrix());
}

} // namespace vr
