#include "gfx/FullscreenQuad.h"

#include <cstddef>
#include <utility>

#include <glad/gl.h>

namespace vr {

namespace {

// Four vertices drawn as a triangle strip cover the viewport. Each is an
// interleaved clip-space position (xy) followed by a UV (st). The UV runs 0->1
// across the quad so the debug fragment shader shows a clean gradient, and so
// later passes have a per-pixel parameter to reconstruct rays from.
//
// clang-format off
constexpr float kQuadVertices[] = {
    // position      uv
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
};
// clang-format on

constexpr GLsizei kVertexCount = 4;
constexpr GLsizei kStride = 4 * sizeof(float);

} // namespace

FullscreenQuad::FullscreenQuad() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // aPosition
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, kStride, nullptr);
    glEnableVertexAttribArray(1); // aUv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, kStride,
                          reinterpret_cast<const void*>(2 * sizeof(float)));

    glBindVertexArray(0);
}

FullscreenQuad::~FullscreenQuad() {
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
}

FullscreenQuad::FullscreenQuad(FullscreenQuad&& other) noexcept
    : vao_(other.vao_), vbo_(other.vbo_) {
    other.vao_ = 0;
    other.vbo_ = 0;
}

FullscreenQuad& FullscreenQuad::operator=(FullscreenQuad&& other) noexcept {
    if (this != &other) {
        if (vbo_ != 0) {
            glDeleteBuffers(1, &vbo_);
        }
        if (vao_ != 0) {
            glDeleteVertexArrays(1, &vao_);
        }
        vao_ = std::exchange(other.vao_, 0u);
        vbo_ = std::exchange(other.vbo_, 0u);
    }
    return *this;
}

void FullscreenQuad::draw() const {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, kVertexCount);
    glBindVertexArray(0);
}

} // namespace vr
