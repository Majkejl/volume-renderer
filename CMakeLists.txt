cmake_minimum_required(VERSION 3.21)

project(volume_renderer
    VERSION 0.1.0
    DESCRIPTION "Real-time GPU volume renderer (OpenGL 4.3 ray-casting)"
    LANGUAGES CXX)

# --- Language standard -------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Emit compile_commands.json for tooling (clang-format/clangd/CI).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)

# --- Dependencies (provided by Conan via CMakeDeps) --------------------------
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(GTest REQUIRED)

# --- Main executable ---------------------------------------------------------
add_executable(volume_renderer
    src/main.cpp
    src/gfx/Camera.cpp
    src/gfx/FullscreenQuad.cpp
    src/gfx/Shader.cpp
    src/gfx/ShaderSource.cpp
    src/gfx/Texture3D.cpp
    src/volume/SyntheticVolume.cpp
    src/volume/VolumeData.cpp)
target_include_directories(volume_renderer PRIVATE src)

# Bake the source-tree shaders directory in so the app loads GLSL straight from
# the build tree; ShaderSource falls back to a "shaders/" dir for deployment.
target_compile_definitions(volume_renderer
    PRIVATE VR_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

target_link_libraries(volume_renderer
    PRIVATE
        glfw
        glad::glad
        glm::glm
        imgui::imgui)
vr_set_warnings(volume_renderer)

# --- Tests -------------------------------------------------------------------
enable_testing()
add_subdirectory(tests)
