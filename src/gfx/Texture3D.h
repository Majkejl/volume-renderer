#pragma once

#include <glad/gl.h>

namespace vr {

class VolumeData;

/// \brief A 3D GL texture holding a normalized scalar volume as GL_R16.
///
/// Owns the texture handle (RAII), move-only. Uses trilinear filtering and
/// clamp-to-edge wrapping on all three axes, so ray samples outside [0,1] read
/// the boundary value rather than wrapping around.
class Texture3D {
  public:
    /// Upload \p volume (normalized to 16-bit) into a new GL 3D texture.
    explicit Texture3D(const VolumeData& volume);
    ~Texture3D();

    Texture3D(const Texture3D&) = delete;
    Texture3D& operator=(const Texture3D&) = delete;
    Texture3D(Texture3D&& other) noexcept;
    Texture3D& operator=(Texture3D&& other) noexcept;

    /// Bind to the given texture unit (e.g. GL_TEXTURE0) on the 3D target.
    void bind(GLenum textureUnit) const;

    GLuint id() const { return texture_; }

  private:
    GLuint texture_ = 0;
};

} // namespace vr
