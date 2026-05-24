#pragma once

#include <glm/glm.hpp>

namespace vr {

/// Result of a ray–box intersection: whether the ray hits, and the entry/exit
/// ray parameters. On a hit, \c tNear is clamped to >= 0 so a ray starting
/// inside the box marches from its origin.
struct RayBoxHit {
    bool hit;
    float tNear;
    float tFar;
};

/// \brief Slab-method intersection of a ray with an axis-aligned box.
///
/// This is the host-side reference for the slab test in shaders/raycast.frag;
/// the two must stay in sync. A ray misses when the entry parameter exceeds the
/// exit one, or the box lies entirely behind the origin.
///
/// \note Rays exactly parallel to and on a slab boundary produce 0*inf = NaN,
///       which is not handled (it does not arise for the camera-driven rays).
inline RayBoxHit intersectRayBox(const glm::vec3& origin, const glm::vec3& dir,
                                 const glm::vec3& boxMin, const glm::vec3& boxMax) {
    const glm::vec3 invDir = 1.0f / dir;
    const glm::vec3 t0 = (boxMin - origin) * invDir;
    const glm::vec3 t1 = (boxMax - origin) * invDir;
    const glm::vec3 tSmall = glm::min(t0, t1);
    const glm::vec3 tBig = glm::max(t0, t1);

    const float tNear = glm::max(glm::max(tSmall.x, tSmall.y), tSmall.z);
    const float tFar = glm::min(glm::min(tBig.x, tBig.y), tBig.z);

    if (tNear > tFar || tFar < 0.0f) {
        return RayBoxHit{false, tNear, tFar};
    }
    return RayBoxHit{true, glm::max(tNear, 0.0f), tFar};
}

} // namespace vr
