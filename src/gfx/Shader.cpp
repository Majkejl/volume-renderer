#include "gfx/Shader.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gfx/ShaderSource.h"

namespace vr {

namespace {

// Read a shader object's info log into a string, trimming the trailing NUL the
// driver writes so it concatenates cleanly into an error message.
std::string shaderInfoLog(GLuint shader) {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log(static_cast<std::size_t>(length), '\0');
    GLsizei written = 0;
    glGetShaderInfoLog(shader, length, &written, log.data());
    log.resize(static_cast<std::size_t>(written));
    return log;
}

std::string programInfoLog(GLuint program) {
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::string log(static_cast<std::size_t>(length), '\0');
    GLsizei written = 0;
    glGetProgramInfoLog(program, length, &written, log.data());
    log.resize(static_cast<std::size_t>(written));
    return log;
}

// Compile one shader stage from source; throws with file context on failure.
GLuint compileStage(GLenum stage, const std::string& source, const std::filesystem::path& path) {
    const GLuint shader = glCreateShader(stage);
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        const std::string log = shaderInfoLog(shader);
        glDeleteShader(shader);
        throw std::runtime_error("Failed to compile shader " + path.string() + ":\n" + log);
    }
    return shader;
}

} // namespace

Shader::Shader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath) {
    const std::string vertSource = readTextFile(vertPath);
    const std::string fragSource = readTextFile(fragPath);

    const GLuint vert = compileStage(GL_VERTEX_SHADER, vertSource, vertPath);
    GLuint frag = 0;
    try {
        frag = compileStage(GL_FRAGMENT_SHADER, fragSource, fragPath);
    } catch (...) {
        glDeleteShader(vert); // don't leak the vertex shader if the fragment fails
        throw;
    }

    program_ = glCreateProgram();
    glAttachShader(program_, vert);
    glAttachShader(program_, frag);
    glLinkProgram(program_);

    // The stage objects are no longer needed once the program is linked.
    glDetachShader(program_, vert);
    glDetachShader(program_, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    GLint linked = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        const std::string log = programInfoLog(program_);
        glDeleteProgram(program_);
        program_ = 0;
        throw std::runtime_error("Failed to link program (" + vertPath.string() + " + " +
                                 fragPath.string() + "):\n" + log);
    }
}

Shader::~Shader() {
    if (program_ != 0) {
        glDeleteProgram(program_);
    }
}

Shader::Shader(Shader&& other) noexcept : program_(other.program_) {
    other.program_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (program_ != 0) {
            glDeleteProgram(program_);
        }
        program_ = std::exchange(other.program_, 0u);
    }
    return *this;
}

void Shader::use() const {
    glUseProgram(program_);
}

GLint Shader::uniformLocation(const std::string& name) const {
    return glGetUniformLocation(program_, name.c_str());
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(uniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(uniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

} // namespace vr
