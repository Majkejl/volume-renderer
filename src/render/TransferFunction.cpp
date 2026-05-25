#include "render/TransferFunction.h"

#include <algorithm>

namespace vr {

namespace {

std::uint8_t toByte(float value) {
    const float clamped = std::clamp(value, 0.0f, 1.0f);
    return static_cast<std::uint8_t>(clamped * 255.0f + 0.5f);
}

} // namespace

std::vector<std::uint8_t> buildTransferLut(const std::vector<TransferControlPoint>& points,
                                           std::size_t size) {
    std::vector<std::uint8_t> lut(size * 4, std::uint8_t{0});
    if (points.empty() || size == 0) {
        return lut;
    }

    for (std::size_t i = 0; i < size; ++i) {
        const float position =
            (size == 1) ? 0.0f : static_cast<float>(i) / static_cast<float>(size - 1);

        glm::vec4 color;
        if (position <= points.front().position) {
            color = points.front().color;
        } else if (position >= points.back().position) {
            color = points.back().color;
        } else {
            // Points are sorted, so advance to the first one at/after this
            // density; it and its predecessor bound the interpolation segment.
            std::size_t hi = 1;
            while (hi < points.size() && points[hi].position < position) {
                ++hi;
            }
            const TransferControlPoint& a = points[hi - 1];
            const TransferControlPoint& b = points[hi];
            const float span = b.position - a.position;
            const float t = (span > 0.0f) ? (position - a.position) / span : 0.0f;
            color = glm::mix(a.color, b.color, t);
        }

        lut[i * 4 + 0] = toByte(color.r);
        lut[i * 4 + 1] = toByte(color.g);
        lut[i * 4 + 2] = toByte(color.b);
        lut[i * 4 + 3] = toByte(color.a);
    }
    return lut;
}

} // namespace vr
