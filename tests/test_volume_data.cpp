#include "volume/VolumeData.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

TEST(VolumeDataTest, StoresDimensions) {
    const vr::VolumeData volume(2, 3, 4, std::vector<float>(2 * 3 * 4, 0.0f));
    EXPECT_EQ(volume.nx(), 2u);
    EXPECT_EQ(volume.ny(), 3u);
    EXPECT_EQ(volume.nz(), 4u);
    EXPECT_EQ(volume.voxelCount(), 24u);
}

TEST(VolumeDataTest, ComputesMinMaxFromVoxels) {
    const vr::VolumeData volume(4, 1, 1, std::vector<float>{-1.0f, 0.0f, 2.5f, 4.0f});
    EXPECT_FLOAT_EQ(volume.minValue(), -1.0f);
    EXPECT_FLOAT_EQ(volume.maxValue(), 4.0f);
}

TEST(VolumeDataTest, IndexesXFastest) {
    // Fill so each voxel equals its linear index = x + nx*(y + ny*z).
    std::vector<float> voxels(2 * 2 * 2);
    for (std::size_t i = 0; i < voxels.size(); ++i) {
        voxels[i] = static_cast<float>(i);
    }
    const vr::VolumeData volume(2, 2, 2, voxels);

    EXPECT_FLOAT_EQ(volume.at(0, 0, 0), 0.0f);
    EXPECT_FLOAT_EQ(volume.at(1, 0, 0), 1.0f); // x advances first
    EXPECT_FLOAT_EQ(volume.at(0, 1, 0), 2.0f);
    EXPECT_FLOAT_EQ(volume.at(0, 0, 1), 4.0f);
    EXPECT_FLOAT_EQ(volume.at(1, 1, 1), 7.0f);
}

TEST(VolumeDataTest, NormalizesRangeToFull16Bit) {
    const vr::VolumeData volume(5, 1, 1, std::vector<float>{0.0f, 1.0f, 2.0f, 3.0f, 4.0f});
    const std::vector<std::uint16_t> u16 = volume.toNormalizedU16();

    ASSERT_EQ(u16.size(), 5u);
    EXPECT_EQ(static_cast<int>(u16.front()), 0);    // min -> 0
    EXPECT_EQ(static_cast<int>(u16.back()), 65535); // max -> 65535
    EXPECT_EQ(static_cast<int>(u16[2]), 32768);     // midpoint -> half scale
}

TEST(VolumeDataTest, ConstantVolumeNormalizesToZero) {
    const vr::VolumeData volume(3, 1, 1, std::vector<float>{7.0f, 7.0f, 7.0f});
    const std::vector<std::uint16_t> u16 = volume.toNormalizedU16();
    for (const std::uint16_t value : u16) {
        EXPECT_EQ(static_cast<int>(value), 0);
    }
}
