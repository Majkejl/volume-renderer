#pragma once

#include <glm/glm.hpp>

namespace vr {

/// \brief An orbit (arcball-style) camera that rotates around a fixed target.
///
/// Parameterized by azimuth, elevation, and distance. Produces the view and
/// projection matrices plus the inverse view-projection used to reconstruct a
/// world-space ray per pixel. Pure math — no OpenGL dependency, so it is
/// unit-testable without a GL context.
class Camera {
  public:
    Camera();

    /// Rotate around the target. Deltas are in radians (typically derived from
    /// mouse motion). Elevation is clamped to keep the camera off the poles, so
    /// the up vector never aligns with the view direction.
    void orbit(float deltaAzimuth, float deltaElevation);

    /// Dolly toward/away from the target. \p steps is signed (e.g. scroll
    /// notches); positive zooms in. Distance is clamped to a sane range.
    void zoom(float steps);

    /// Set the viewport aspect ratio (width / height) used by the projection.
    /// Non-positive values are ignored (e.g. a minimized window).
    void setAspect(float aspect);

    /// World-space eye position.
    glm::vec3 position() const;

    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix() const;
    glm::mat4 viewProjectionMatrix() const;

    /// Inverse of the view-projection, for unprojecting NDC back to world space.
    glm::mat4 inverseViewProjection() const;

  private:
    glm::vec3 target_;
    float azimuth_;   // radians, around +Y
    float elevation_; // radians, from the XZ plane (clamped)
    float distance_;

    float fovY_; // radians, vertical field of view
    float aspect_;
    float nearPlane_;
    float farPlane_;
};

} // namespace vr
