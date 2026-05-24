#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace vr {

/// \brief A scalar volume in memory: dimensions, voxel values, and value range.
///
/// Voxels are stored as float (the canonical in-memory scalar type) in
/// x-fastest order: index = x + nx*(y + ny*z). The range [minValue, maxValue]
/// is in source units; a texture upload normalizes against it, and the UI later
/// windows in those units. Real-data loaders convert their on-disk type to
/// float and feed this same representation.
class VolumeData {
  public:
    /// \param voxels exactly nx*ny*nz values in x-fastest order. The value range
    ///        is computed from the data.
    VolumeData(std::size_t nx, std::size_t ny, std::size_t nz, std::vector<float> voxels);

    std::size_t nx() const { return nx_; }
    std::size_t ny() const { return ny_; }
    std::size_t nz() const { return nz_; }
    std::size_t voxelCount() const { return voxels_.size(); }

    const std::vector<float>& voxels() const { return voxels_; }

    float minValue() const { return minValue_; }
    float maxValue() const { return maxValue_; }

    /// Voxel value at integer coordinates (asserts the indices are in range).
    float at(std::size_t x, std::size_t y, std::size_t z) const;

    /// Voxels normalized from [minValue, maxValue] to the full 16-bit range,
    /// ready to upload as a GL_R16 texture. A constant volume maps to all zeros.
    std::vector<std::uint16_t> toNormalizedU16() const;

  private:
    std::size_t nx_;
    std::size_t ny_;
    std::size_t nz_;
    std::vector<float> voxels_;
    float minValue_;
    float maxValue_;
};

} // namespace vr
