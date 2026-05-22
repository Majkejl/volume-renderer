// Volume Renderer — application entry point.
//
// Milestone M0: open an OpenGL 4.3 core-profile window and clear it to a solid
// colour every frame. This file intentionally contains no abstractions yet; the
// Shader / Camera / VolumeRenderer classes are introduced in later phases.

#include <cstdlib>
#include <iostream>

#include <glad/gl.h> // must precede the GLFW header

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace {

constexpr int kInitialWidth = 1280;
constexpr int kInitialHeight = 720;

// A distinct, non-black clear colour so a successful frame is unmistakable
// (the M0 exit criterion). Muted slate blue, RGBA in [0,1].
constexpr float kClearColor[4] = {0.12f, 0.14f, 0.18f, 1.0f};

void glfwErrorCallback(int code, const char* description) {
    std::cerr << "[GLFW] error " << code << ": " << description << '\n';
}

#ifndef NDEBUG
// KHR_debug callback (core since OpenGL 4.3). Debug builds only: it routes the
// driver's diagnostics to stderr so GL errors surface with context instead of
// failing silently. APIENTRY matches the GLDEBUGPROC calling convention.
void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei /*length*/, const GLchar* message,
                              const void* /*userParam*/) {
    // Notifications are noisy and rarely actionable; drop them.
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }
    std::cerr << "[GL] " << message << " (source=" << source << " type=" << type << " id=" << id
              << " severity=" << severity << ")\n";
}
#endif

} // namespace

int main() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initialize GLFW\n";
        return EXIT_FAILURE;
    }

    // Request an OpenGL 4.3 core-profile context.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    GLFWwindow* window =
        glfwCreateWindow(kInitialWidth, kInitialHeight, "Volume Renderer", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window (OpenGL 4.3 core profile required)\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync on

    const int glVersion = gladLoadGL(glfwGetProcAddress);
    if (glVersion == 0) {
        std::cerr << "Failed to load OpenGL functions via glad\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

#ifndef NDEBUG
    // Only wire up the callback if we actually got a debug context. Synchronous
    // output fires the callback on the offending call (easier debugging, slower)
    // — acceptable since this branch is debug-only.
    GLint contextFlags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if ((contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
#endif

    std::cout << "OpenGL " << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << " | "
              << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << '\n';

    glClearColor(kClearColor[0], kClearColor[1], kClearColor[2], kClearColor[3]);

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();

        // ESC closes the window, alongside the title-bar close button.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        int fbWidth = 0;
        int fbHeight = 0;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
