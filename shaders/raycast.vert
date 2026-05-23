#version 430 core

// Fullscreen-quad pass-through vertex shader.
//
// Vertex positions arrive already in clip space ([-1,1] across the viewport),
// so they pass straight to gl_Position. The accompanying UV (0->1 across the
// quad) is forwarded to the fragment shader: a debug coordinate for now, and
// the basis for per-pixel ray reconstruction in later milestones.

layout(location = 0) in vec2 aPosition; // clip-space XY
layout(location = 1) in vec2 aUv;       // [0,1] across the viewport

out vec2 vUv;

void main() {
    vUv = aUv;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
