#include "gfx/Texture3D.h"

#include <cstdint>
#include <utility>
#include <vector>

#include <glad/gl.h>

#include "volume/VolumeData.h"

namespace vr {

Texture3D::Texture3D(const VolumeData& volume) {
    const std::vector<std::uint16_t> normalized = volume.toNormalizedU16();

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_3D, texture_);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Single-channel 16-bit rows: relax the 4-byte default unpack alignment so
    // odd-width volumes upload without row misalignment.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R16, static_cast<GLsizei>(volume.nx()),
                 static_cast<GLsizei>(volume.ny()), static_cast<GLsizei>(volume.nz()), 0, GL_RED,
                 GL_UNSIGNED_SHORT, normalized.data());

    glBindTexture(GL_TEXTURE_3D, 0);
}

Texture3D::~Texture3D() {
    if (texture_ != 0) {
        glDeleteTextures(1, &texture_);
    }
}

Texture3D::Texture3D(Texture3D&& other) noexcept : texture_(other.texture_) {
    other.texture_ = 0;
}

Texture3D& Texture3D::operator=(Texture3D&& other) noexcept {
    if (this != &other) {
        if (texture_ != 0) {
            glDeleteTextures(1, &texture_);
        }
        texture_ = std::exchange(other.texture_, 0u);
    }
    return *this;
}

void Texture3D::bind(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_3D, texture_);
}

} // namespace vr
