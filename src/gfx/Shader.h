#pragma once

#include <filesystem>
#include <string>

#include <glad/gl.h>
#include <glm/fwd.hpp>

namespace vr {

/// \brief An OpenGL shader program built from a vertex + fragment shader pair.
///
/// Owns the linked GL program object and deletes it on destruction (RAII).
/// Move-only: copying would let two instances delete the same GL handle.
class Shader {
  public:
    /// Compile and link a program from two GLSL source files on disk.
    /// \throws std::runtime_error on file-read, compile, or link failure. The
    ///         message carries the file path and the driver info log (which
    ///         includes GLSL line numbers) so errors are diagnosable.
    Shader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    /// Bind this program for subsequent draw calls.
    void use() const;

    GLuint id() const { return program_; }

    // Uniform setters, located by name on each call. Simple and adequate here;
    // location caching can come later if a hot path needs it.
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

  private:
    GLint uniformLocation(const std::string& name) const;

    GLuint program_ = 0;
};

} // namespace vr
