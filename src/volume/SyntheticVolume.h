#pragma once

#include <cstddef>

#include "volume/VolumeData.h"

namespace vr {

/// \brief Generate a radial Gaussian blob volume, centered in the cube.
///
/// value(p) = exp(-|p - 0.5|^2 / (2 sigma^2)) for voxel-center positions p in
/// normalized [0,1]^3. It is instantly recognizable for visual bring-up and has
/// an analytic gradient, which later validates the shader's central-difference
/// gradient against ground truth.
///
/// \param sigma Gaussian width in normalized units.
VolumeData makeGaussianBlob(std::size_t nx, std::size_t ny, std::size_t nz, float sigma = 0.2f);

} // namespace vr
