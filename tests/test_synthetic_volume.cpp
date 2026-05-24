#include "volume/SyntheticVolume.h"

#include "volume/VolumeData.h"

#include <gtest/gtest.h>

// Odd dimensions place a voxel center exactly at the cube center (0.5), where
// the Gaussian peaks at exp(0) = 1 — a convenient known value to test against.

TEST(SyntheticVolumeTest, HasRequestedDimensions) {
    const vr::VolumeData volume = vr::makeGaussianBlob(9, 9, 9);
    EXPECT_EQ(volume.nx(), 9u);
    EXPECT_EQ(volume.ny(), 9u);
    EXPECT_EQ(volume.nz(), 9u);
    EXPECT_EQ(volume.voxelCount(), 729u);
}

TEST(SyntheticVolumeTest, PeaksAtCenter) {
    const vr::VolumeData volume = vr::makeGaussianBlob(9, 9, 9);
    EXPECT_NEAR(volume.at(4, 4, 4), 1.0f, 1e-6f);
    EXPECT_NEAR(volume.maxValue(), 1.0f, 1e-6f);
}

TEST(SyntheticVolumeTest, FallsOffRadially) {
    const vr::VolumeData volume = vr::makeGaussianBlob(9, 9, 9);
    const float center = volume.at(4, 4, 4);
    const float midway = volume.at(2, 4, 4);
    const float corner = volume.at(0, 0, 0);

    EXPECT_GT(center, midway);
    EXPECT_GT(midway, corner);
    EXPECT_LT(corner, 0.1f); // tails fall near zero
}

TEST(SyntheticVolumeTest, IsSymmetricAcrossCenter) {
    const vr::VolumeData volume = vr::makeGaussianBlob(9, 9, 9);
    EXPECT_NEAR(volume.at(1, 4, 4), volume.at(7, 4, 4), 1e-6f); // mirror in x
    EXPECT_NEAR(volume.at(4, 1, 4), volume.at(4, 7, 4), 1e-6f); // mirror in y
    EXPECT_NEAR(volume.at(4, 4, 1), volume.at(4, 4, 7), 1e-6f); // mirror in z
}
