#version 430 core

// Milestone M1 placeholder fragment shader.
//
// Outputs the interpolated UV as colour (red = u, green = v) to prove the
// fullscreen quad and the disk-loaded shader pipeline work end to end. The
// ray-caster replaces this in a later milestone.

in vec2 vUv;

out vec4 fragColor;

void main() {
    fragColor = vec4(vUv, 0.0, 1.0);
}
