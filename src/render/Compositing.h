#pragma once

#include <glm/glm.hpp>

namespace vr {

/// \brief Accumulate one front-to-back compositing step (premultiplied alpha).
///
/// Host-side reference for the compositing in shaders/raycast.frag; the two must
/// stay in sync. \p dst is the running result (RGB premultiplied by alpha,
/// alpha = accumulated opacity); \p src is the current sample's colour/opacity.
/// Front-to-back order is what allows early-ray-termination once dst.a saturates.
inline void compositeFrontToBack(glm::vec4& dst, const glm::vec4& src) {
    const float transmittance = 1.0f - dst.a; // light still passing through
    dst.r += transmittance * src.a * src.r;
    dst.g += transmittance * src.a * src.g;
    dst.b += transmittance * src.a * src.b;
    dst.a += transmittance * src.a;
}

} // namespace vr
