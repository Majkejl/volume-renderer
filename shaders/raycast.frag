#version 430 core

// Milestone M4 fragment shader: single-pass volume ray-casting.
//
// For each pixel: reconstruct a world-space ray from the camera, shift it into
// the volume's texture space, intersect the unit box, then march front-to-back
// accumulating colour and opacity (emission–absorption). Density maps directly
// to greyscale + opacity here; a transfer function and lighting come later.
//
// Spaces: vUv is [0,1] across the viewport -> NDC [-1,1]. The volume is the unit
// cube centred at the world origin, so texture space = world + 0.5. Texture
// coordinates and the ray parameter t are both in that [0,1]^3 space.

in vec2 vUv;

out vec4 fragColor;

uniform sampler3D uVolume;
uniform mat4 uInvViewProj; // inverse(projection * view)
uniform vec3 uCameraPos;   // world-space eye
uniform float uStepSize;   // march step in texture-space units

const vec3 kBoxMin = vec3(0.0);
const vec3 kBoxMax = vec3(1.0);
const vec3 kBackground = vec3(0.04, 0.05, 0.07);

// Slab-method ray–box intersection (mirrors render/RayBox.h). On a hit, tNear is
// clamped to >= 0 so a camera inside the box marches from its own position.
bool intersectBox(vec3 origin, vec3 dir, out float tNear, out float tFar) {
    vec3 invDir = 1.0 / dir;
    vec3 t0 = (kBoxMin - origin) * invDir;
    vec3 t1 = (kBoxMax - origin) * invDir;
    vec3 tSmall = min(t0, t1);
    vec3 tBig = max(t0, t1);
    tNear = max(max(tSmall.x, tSmall.y), tSmall.z);
    tFar = min(min(tBig.x, tBig.y), tBig.z);
    if (tNear > tFar || tFar < 0.0) {
        return false;
    }
    tNear = max(tNear, 0.0);
    return true;
}

void main() {
    // Reconstruct the world ray, then translate the origin into texture space.
    vec2 ndc = vUv * 2.0 - 1.0;
    vec4 farHomogeneous = uInvViewProj * vec4(ndc, 1.0, 1.0);
    vec3 farPoint = farHomogeneous.xyz / farHomogeneous.w;
    vec3 rayDir = normalize(farPoint - uCameraPos);
    vec3 rayOrigin = uCameraPos + vec3(0.5);

    float tNear;
    float tFar;
    if (!intersectBox(rayOrigin, rayDir, tNear, tFar)) {
        fragColor = vec4(kBackground, 1.0);
        return;
    }

    // Front-to-back accumulation (premultiplied alpha) with early termination.
    vec4 dst = vec4(0.0);
    for (float t = tNear; t < tFar; t += uStepSize) {
        vec3 pos = rayOrigin + t * rayDir;
        float density = texture(uVolume, pos).r;

        // Placeholder mapping: greyscale colour, opacity proportional to density.
        vec4 src = vec4(vec3(density), density);

        dst.rgb += (1.0 - dst.a) * src.a * src.rgb;
        dst.a += (1.0 - dst.a) * src.a;
        if (dst.a >= 0.99) {
            break; // early ray termination
        }
    }

    // Composite the accumulated colour over the background.
    vec3 color = dst.rgb + (1.0 - dst.a) * kBackground;
    fragColor = vec4(color, 1.0);
}
