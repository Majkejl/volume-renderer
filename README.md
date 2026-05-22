# Volume Renderer

A real-time GPU volume renderer in **C++17 / OpenGL 4.3 / GLSL**. It ray-casts a
scalar volume (synthetic first, real electron-microscopy data later) in a single
fragment-shader pass through a 3D texture, with a transfer function,
gradient-based lighting, an orbit camera, and an interactive parameter UI.

> **Status:** early scaffolding (M0). The window and build system are in place;
> the ray-caster, transfer function, lighting, and UI land in subsequent
> milestones. Screenshots and dataset provenance will be added as features land.

## Requirements

- A C++17 compiler (MSVC 2022 on Windows; GCC/Clang on Linux)
- [CMake](https://cmake.org/) ≥ 3.21
- [Conan](https://conan.io/) 2.x (`pip install "conan>=2.0,<3"`)
- A GPU/driver supporting **OpenGL 4.3 core profile**

## Build

```sh
# 1. One-time: create a Conan profile for your toolchain
conan profile detect --force

# 2. Resolve and build dependencies (C++17 to match the project standard)
conan install . --output-folder=build --build=missing -s compiler.cppstd=17

# 3. Configure and build
cmake --preset conan-default
cmake --build build --config Release

# 4. Run the tests
ctest --test-dir build -C Release --output-on-failure
```

The executable is produced under `build/` (e.g. `build/Release/volume_renderer`
on Windows). Press **Esc** or close the window to exit.

## Dependencies

All third-party libraries are resolved through Conan (`conanfile.txt`):

| Library  | Purpose                          |
|----------|----------------------------------|
| GLFW     | Windowing and input              |
| glad     | OpenGL 4.3 core function loader   |
| GLM      | Vector/matrix math               |
| Dear ImGui | Interactive parameter UI       |
| GoogleTest | Unit tests (via CTest)         |

## License

MIT — see [LICENSE](LICENSE).
