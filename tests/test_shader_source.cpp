#include "gfx/ShaderSource.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

// readTextFile is the GL-free half of shader loading, so it can be tested
// without a GL context. Tests write a temp file and read it back, then verify
// the failure path reports the path it could not open.

TEST(ShaderSourceTest, ReadsFileContentsVerbatim) {
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "vr_read_text_file_test.glsl";
    const std::string expected = "#version 430 core\nvoid main() {}\n";
    {
        std::ofstream out(path, std::ios::binary);
        ASSERT_TRUE(out.good()) << "could not create temp file " << path;
        out << expected;
    }

    EXPECT_EQ(vr::readTextFile(path), expected);

    std::filesystem::remove(path);
}

TEST(ShaderSourceTest, ThrowsOnMissingFile) {
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "vr_definitely_missing_file.glsl";
    std::filesystem::remove(path); // make sure it is absent

    EXPECT_THROW(vr::readTextFile(path), std::runtime_error);
}

TEST(ShaderSourceTest, MissingFileErrorIncludesPath) {
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "vr_missing_with_context.glsl";
    std::filesystem::remove(path);

    try {
        vr::readTextFile(path);
        FAIL() << "expected readTextFile to throw on a missing file";
    } catch (const std::runtime_error& error) {
        EXPECT_NE(std::string(error.what()).find("vr_missing_with_context.glsl"),
                  std::string::npos);
    }
}
