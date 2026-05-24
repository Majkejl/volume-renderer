// Volume Renderer — application entry point.
//
// Creates an OpenGL 4.3 core-profile window, builds the shader pipeline and a
// fullscreen quad, generates a synthetic volume and uploads it to a 3D texture,
// then ray-marches it in the fragment shader with a mouse-driven orbit camera.

#include <cstdlib>
#include <exception>
#include <iostream>

#include <glad/gl.h> // must precede the GLFW header

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "gfx/Camera.h"
#include "gfx/FullscreenQuad.h"
#include "gfx/Shader.h"
#include "gfx/ShaderSource.h"
#include "gfx/Texture3D.h"
#include "volume/SyntheticVolume.h"
#include "volume/VolumeData.h"

// On systems with switchable graphics, ask the driver to pick the
// high-performance discrete GPU. Otherwise the GL context may be created on the
// integrated GPU; if the display is driven by the discrete GPU, the window then
// presents black (rendering succeeds, but on the wrong adapter). These exported
// symbols are read by the NVIDIA / AMD drivers at process start, ignored on
// single-GPU systems. They must be exported from the executable itself.
#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace {

constexpr int kInitialWidth = 1280;
constexpr int kInitialHeight = 720;

// A distinct, non-black clear colour so a successful frame is unmistakable
// (the M0 exit criterion). Muted slate blue, RGBA in [0,1].
constexpr float kClearColor[4] = {0.12f, 0.14f, 0.18f, 1.0f};

// Ray-march step in texture-space units (the volume is a unit cube). With the
// placeholder density-as-opacity mapping the image is step-dependent (a later
// milestone adds opacity correction); this default gives a clear cloud. The UI
// exposes it later too.
constexpr float kStepSize = 0.02f;

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

// Mouse-driven orbit state. It lives behind the GLFW window user pointer so the
// event-driven (hence frame-rate-independent) input callbacks can reach the
// camera without globals.
struct OrbitInput {
    vr::Camera* camera = nullptr;
    bool dragging = false;
    double lastX = 0.0;
    double lastY = 0.0;
};

OrbitInput* inputFrom(GLFWwindow* window) {
    return static_cast<OrbitInput*>(glfwGetWindowUserPointer(window));
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    OrbitInput* input = inputFrom(window);
    if (input == nullptr) {
        return;
    }
    if (input->dragging && input->camera != nullptr) {
        const float dx = static_cast<float>(x - input->lastX);
        const float dy = static_cast<float>(y - input->lastY);
        constexpr float kRadiansPerPixel = 0.005f; // drag sensitivity
        input->camera->orbit(-dx * kRadiansPerPixel, -dy * kRadiansPerPixel);
    }
    input->lastX = x;
    input->lastY = y;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }
    OrbitInput* input = inputFrom(window);
    if (input == nullptr) {
        return;
    }
    if (action == GLFW_PRESS) {
        input->dragging = true;
        glfwGetCursorPos(window, &input->lastX, &input->lastY); // anchor the drag
    } else if (action == GLFW_RELEASE) {
        input->dragging = false;
    }
}

void scrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    OrbitInput* input = inputFrom(window);
    if (input != nullptr && input->camera != nullptr) {
        input->camera->zoom(static_cast<float>(yoffset));
    }
}

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

    vr::Camera camera;
    OrbitInput input;
    input.camera = &camera;
    glfwSetWindowUserPointer(window, &input);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    int exitCode = EXIT_SUCCESS;
    try {
        // Build the shader program and quad while the context is current. The
        // enclosing scope guarantees these GL resources are destroyed (their
        // destructors call glDelete*) before the context is torn down below.
        const vr::Shader shader(vr::shaderPath("raycast.vert"), vr::shaderPath("raycast.frag"));
        const vr::FullscreenQuad quad;

        // Synthetic volume uploaded to a 3D texture and ray-marched in the
        // fragment shader; the orbit camera drives the per-pixel rays.
        const vr::VolumeData volume = vr::makeGaussianBlob(128, 128, 128);
        const vr::Texture3D volumeTexture(volume);

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
            if (fbHeight > 0) {
                camera.setAspect(static_cast<float>(fbWidth) / static_cast<float>(fbHeight));
            }

            glClear(GL_COLOR_BUFFER_BIT);

            shader.use();
            volumeTexture.bind(GL_TEXTURE0);
            shader.setInt("uVolume", 0);
            shader.setMat4("uInvViewProj", camera.inverseViewProjection());
            shader.setVec3("uCameraPos", camera.position());
            shader.setFloat("uStepSize", kStepSize);
            quad.draw();

            glfwSwapBuffers(window);
        }
    } catch (const std::exception& error) {
        // Setup boundaries (file read, shader compile/link) may throw; report
        // with context and exit cleanly instead of terminating.
        std::cerr << "Fatal: " << error.what() << '\n';
        exitCode = EXIT_FAILURE;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return exitCode;
}
