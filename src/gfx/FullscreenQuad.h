#pragma once

#include <glad/gl.h>

namespace vr {

/// \brief A screen-covering quad used to run a fragment shader over every pixel.
///
/// Owns its VAO + VBO and deletes them on destruction (RAII). Move-only.
/// This is the surface the ray-caster draws into in later milestones.
class FullscreenQuad {
  public:
    FullscreenQuad();
    ~FullscreenQuad();

    FullscreenQuad(const FullscreenQuad&) = delete;
    FullscreenQuad& operator=(const FullscreenQuad&) = delete;
    FullscreenQuad(FullscreenQuad&& other) noexcept;
    FullscreenQuad& operator=(FullscreenQuad&& other) noexcept;

    /// Draw the quad. A shader program must be bound (Shader::use) beforehand.
    void draw() const;

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
};

} // namespace vr
