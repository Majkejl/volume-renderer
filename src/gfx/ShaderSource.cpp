#include "gfx/ShaderSource.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <system_error>

// Absolute path to the source-tree shaders directory, injected by CMake. The
// fallback lets translation units built without it (e.g. the test target) still
// compile; they resolve shaders relative to the working directory instead.
#ifndef VR_SHADER_DIR
#define VR_SHADER_DIR "shaders"
#endif

namespace vr {

std::filesystem::path shaderPath(std::string_view fileName) {
    namespace fs = std::filesystem;

    const fs::path baked = fs::path(VR_SHADER_DIR) / fileName;
    std::error_code ec;
    if (fs::exists(baked, ec)) {
        return baked;
    }
    return fs::path("shaders") / fileName;
}

std::string readTextFile(const std::filesystem::path& path) {
    // Binary mode: shader text is read verbatim with no newline translation.
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    if (file.bad()) {
        throw std::runtime_error("Failed to read file: " + path.string());
    }
    return contents.str();
}

} // namespace vr
