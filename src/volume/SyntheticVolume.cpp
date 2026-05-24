#include "volume/SyntheticVolume.h"

#include <cmath>
#include <utility>
#include <vector>

namespace vr {

VolumeData makeGaussianBlob(std::size_t nx, std::size_t ny, std::size_t nz, float sigma) {
    std::vector<float> voxels(nx * ny * nz);

    const float twoSigmaSq = 2.0f * sigma * sigma;

    // Voxel-center positions in normalized [0,1] offset to the cube center 0.5.
    std::size_t index = 0;
    for (std::size_t z = 0; z < nz; ++z) {
        const float pz = (static_cast<float>(z) + 0.5f) / static_cast<float>(nz) - 0.5f;
        for (std::size_t y = 0; y < ny; ++y) {
            const float py = (static_cast<float>(y) + 0.5f) / static_cast<float>(ny) - 0.5f;
            for (std::size_t x = 0; x < nx; ++x) {
                const float px = (static_cast<float>(x) + 0.5f) / static_cast<float>(nx) - 0.5f;
                const float r2 = px * px + py * py + pz * pz;
                voxels[index++] = std::exp(-r2 / twoSigmaSq);
            }
        }
    }

    return VolumeData(nx, ny, nz, std::move(voxels));
}

} // namespace vr
