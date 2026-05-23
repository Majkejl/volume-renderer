#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace vr {

/// \brief Resolve a shader file name (e.g. "raycast.frag") to a usable path.
///
/// Prefers the shaders directory baked in at build time so the app runs
/// straight from the build tree; falls back to a "shaders/" directory beside
/// the working directory for a deployed layout. Centralizing this keeps every
/// caller from hard-coding paths.
std::filesystem::path shaderPath(std::string_view fileName);

/// \brief Read an entire text file into a string.
/// \throws std::runtime_error if the file cannot be opened or read; the message
///         includes the path so failures are actionable.
///
/// No OpenGL dependency: this is the unit-testable half of shader loading.
std::string readTextFile(const std::filesystem::path& path);

} // namespace vr
