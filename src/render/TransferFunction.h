#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

namespace vr {

/// A transfer-function control point: a normalized density position in [0,1]
/// mapped to a straight (non-premultiplied) RGBA colour. Alpha is opacity.
struct TransferControlPoint {
    float position;
    glm::vec4 color;
};

/// \brief Build an RGBA8 lookup table by linearly interpolating control points.
///
/// Returns \p size texels (4 bytes each) spanning density [0,1], ready to upload
/// as a 1D texture. Control points must be sorted by ascending position; a
/// density below the first / above the last clamps to that endpoint's colour.
/// Empty input yields all-zero (transparent) texels.
std::vector<std::uint8_t> buildTransferLut(const std::vector<TransferControlPoint>& points,
                                           std::size_t size);

} // namespace vr
