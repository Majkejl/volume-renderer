#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <glad/gl.h>

namespace vr {

/// \brief A 1D RGBA8 texture, used for the transfer-function lookup table.
///
/// Owns the texture handle (RAII), move-only. Linear filtering and
/// clamp-to-edge wrapping so a density lookup interpolates between texels and
/// stays in range at the ends.
class Texture1D {
  public:
    /// \param rgba size * 4 bytes of RGBA8 texels.
    Texture1D(const std::vector<std::uint8_t>& rgba, std::size_t size);
    ~Texture1D();

    Texture1D(const Texture1D&) = delete;
    Texture1D& operator=(const Texture1D&) = delete;
    Texture1D(Texture1D&& other) noexcept;
    Texture1D& operator=(Texture1D&& other) noexcept;

    /// Bind to the given texture unit (e.g. GL_TEXTURE1) on the 1D target.
    void bind(GLenum textureUnit) const;

    GLuint id() const { return texture_; }

  private:
    GLuint texture_ = 0;
};

} // namespace vr
