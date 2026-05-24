#version 430 core

// Milestone M3 debug fragment shader.
//
// Samples a fixed axis-aligned slice (z = 0.5) of the volume to confirm the 3D
// texture uploaded correctly and samples in the expected orientation. The
// screen UV maps directly to the slice's XY texture coordinates. The actual
// ray-march through the volume replaces this in a later milestone.

in vec2 vUv;

out vec4 fragColor;

uniform sampler3D uVolume;

void main() {
    float density = texture(uVolume, vec3(vUv, 0.5)).r;
    fragColor = vec4(vec3(density), 1.0);
}
