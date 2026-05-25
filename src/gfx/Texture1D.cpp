#include "gfx/Texture1D.h"

#include <utility>

#include <glad/gl.h>

namespace vr {

Texture1D::Texture1D(const std::vector<std::uint8_t>& rgba, std::size_t size) {
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_1D, texture_);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, static_cast<GLsizei>(size), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, rgba.data());

    glBindTexture(GL_TEXTURE_1D, 0);
}

Texture1D::~Texture1D() {
    if (texture_ != 0) {
        glDeleteTextures(1, &texture_);
    }
}

Texture1D::Texture1D(Texture1D&& other) noexcept : texture_(other.texture_) {
    other.texture_ = 0;
}

Texture1D& Texture1D::operator=(Texture1D&& other) noexcept {
    if (this != &other) {
        if (texture_ != 0) {
            glDeleteTextures(1, &texture_);
        }
        texture_ = std::exchange(other.texture_, 0u);
    }
    return *this;
}

void Texture1D::bind(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_1D, texture_);
}

} // namespace vr
