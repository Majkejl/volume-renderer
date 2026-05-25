#include "render/TransferFunction.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include <gtest/gtest.h>

// The LUT builder is GL-free, so it is unit-tested directly. toByte rounds
// 0.5 -> 128, 1.0 -> 255, 0.0 -> 0.

TEST(TransferFunctionTest, InterpolatesBetweenTwoPoints) {
    const std::vector<vr::TransferControlPoint> points = {
        {0.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        {1.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
    };
    const std::vector<std::uint8_t> lut = vr::buildTransferLut(points, 3);
    ASSERT_EQ(lut.size(), 12u);

    EXPECT_EQ(static_cast<int>(lut[0]), 255); // entry 0 (density 0): red
    EXPECT_EQ(static_cast<int>(lut[2]), 0);
    EXPECT_EQ(static_cast<int>(lut[4]), 128); // entry 1 (density 0.5): midpoint
    EXPECT_EQ(static_cast<int>(lut[6]), 128);
    EXPECT_EQ(static_cast<int>(lut[8]), 0); // entry 2 (density 1): blue
    EXPECT_EQ(static_cast<int>(lut[10]), 255);
}

TEST(TransferFunctionTest, ClampsOutsideControlRange) {
    const std::vector<vr::TransferControlPoint> points = {
        {0.25f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        {0.75f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
    };
    const std::vector<std::uint8_t> lut = vr::buildTransferLut(points, 3);

    EXPECT_EQ(static_cast<int>(lut[0]), 255); // density 0 < 0.25 -> clamps to red
    EXPECT_EQ(static_cast<int>(lut[2]), 0);
    EXPECT_EQ(static_cast<int>(lut[4]), 128); // density 0.5 -> midpoint
    EXPECT_EQ(static_cast<int>(lut[6]), 128);
    EXPECT_EQ(static_cast<int>(lut[8]), 0); // density 1 > 0.75 -> clamps to blue
    EXPECT_EQ(static_cast<int>(lut[10]), 255);
}

TEST(TransferFunctionTest, SinglePointFillsConstant) {
    const std::vector<vr::TransferControlPoint> points = {
        {0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 0.5f)}};
    const std::vector<std::uint8_t> lut = vr::buildTransferLut(points, 4);
    ASSERT_EQ(lut.size(), 16u);
    for (std::size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(static_cast<int>(lut[i * 4 + 0]), 0);
        EXPECT_EQ(static_cast<int>(lut[i * 4 + 1]), 255);
        EXPECT_EQ(static_cast<int>(lut[i * 4 + 2]), 0);
        EXPECT_EQ(static_cast<int>(lut[i * 4 + 3]), 128);
    }
}

TEST(TransferFunctionTest, EmptyControlPointsGiveZeros) {
    const std::vector<std::uint8_t> lut = vr::buildTransferLut({}, 4);
    ASSERT_EQ(lut.size(), 16u);
    for (const std::uint8_t byte : lut) {
        EXPECT_EQ(static_cast<int>(byte), 0);
    }
}

TEST(TransferFunctionTest, ProducesRequestedSize) {
    const std::vector<vr::TransferControlPoint> points = {{0.0f, glm::vec4(1.0f)},
                                                          {1.0f, glm::vec4(1.0f)}};
    EXPECT_EQ(vr::buildTransferLut(points, 256).size(), 256u * 4u);
}
