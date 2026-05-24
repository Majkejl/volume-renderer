#include "volume/VolumeData.h"

#include <algorithm>
#include <cassert>

namespace vr {

namespace {
constexpr float kU16Max = 65535.0f;
}

VolumeData::VolumeData(std::size_t nx, std::size_t ny, std::size_t nz, std::vector<float> voxels)
    : nx_(nx), ny_(ny), nz_(nz), voxels_(std::move(voxels)), minValue_(0.0f), maxValue_(0.0f) {
    assert(nx_ * ny_ * nz_ == voxels_.size() && "voxel buffer size must equal nx*ny*nz");
    if (!voxels_.empty()) {
        const auto [minIt, maxIt] = std::minmax_element(voxels_.begin(), voxels_.end());
        minValue_ = *minIt;
        maxValue_ = *maxIt;
    }
}

float VolumeData::at(std::size_t x, std::size_t y, std::size_t z) const {
    assert(x < nx_ && y < ny_ && z < nz_);
    return voxels_[x + nx_ * (y + ny_ * z)];
}

std::vector<std::uint16_t> VolumeData::toNormalizedU16() const {
    std::vector<std::uint16_t> result(voxels_.size());

    const float range = maxValue_ - minValue_;
    if (range <= 0.0f) {
        // Constant (or empty) volume: nothing to normalize against.
        std::fill(result.begin(), result.end(), std::uint16_t{0});
        return result;
    }

    const float invRange = 1.0f / range;
    for (std::size_t i = 0; i < voxels_.size(); ++i) {
        const float normalized = (voxels_[i] - minValue_) * invRange; // [0,1]
        result[i] = static_cast<std::uint16_t>(normalized * kU16Max + 0.5f);
    }
    return result;
}

} // namespace vr
