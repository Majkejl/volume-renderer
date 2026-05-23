#version 430 core

// Milestone M2 placeholder fragment shader.
//
// Reconstructs a world-space ray per pixel from the camera's inverse
// view-projection and eye position, then visualizes the ray *direction* as
// colour to confirm the rays track the orbit camera. The actual ray-march
// replaces this in a later milestone.
//
// Spaces: vUv is [0,1] across the viewport; it maps to NDC [-1,1]. Unprojecting
// the far-plane NDC point through uInvViewProj gives a world-space point on the
// ray; the direction is that point minus the eye.

in vec2 vUv;

out vec4 fragColor;

uniform mat4 uInvViewProj; // inverse(projection * view)
uniform vec3 uCameraPos;   // world-space eye

void main() {
    vec2 ndc = vUv * 2.0 - 1.0;

    // Far-plane point in NDC (z = +1) unprojected to world space.
    vec4 farHomogeneous = uInvViewProj * vec4(ndc, 1.0, 1.0);
    vec3 farPoint = farHomogeneous.xyz / farHomogeneous.w;

    vec3 rayDir = normalize(farPoint - uCameraPos);

    // Map direction components [-1,1] -> [0,1] so they're visible as colour.
    fragColor = vec4(rayDir * 0.5 + 0.5, 1.0);
}
