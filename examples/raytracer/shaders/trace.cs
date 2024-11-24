#version 460 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout (rgba16f, binding = 0) uniform image2D Image;

// layout (std140, binding = 2) uniform Environment {
    uniform vec4 u_ImageSize; // Width, Height, Width / Height, Height / Width
    uniform uint u_TotalSamples;
    #define MAX_BOUNCES 25
    uniform uint u_MaxBounces;
    uniform uint u_MaxTracesPerFrame;
// };

#include "!render/shaders/environment.glsl"
#define SUN_IDX 0u

#include "!render/shaders/camera.glsl"
uniform float u_CamDepth;

uniform uint u_PathIdx; // The current contiguously rendered frame
uniform uint u_Seed;

struct Material {
    vec3 albedo;
    float roughness;
    vec4 specular; // Color + probability
    vec4 emission; // Color + intensity
    float transmission;
    float IOR;
};
struct Object {
    vec4 position;      // base offset 16, aligned offset 16
    vec4 scale;         // base offset 16, aligned offset 32
    // mat4 transform;     // base offset 16, aligned offset 48 (1)
                        // base offset 16, aligned offset 64 (2)
                        // base offset 16, aligned offset 80 (3)
                        // base offset 16, aligned offset 96 (4)
    // uvec4 meshIdx;       // base offset 4, aligned offset 100
    uvec4 matIdx;        // base offset 4, aligned offset 104
};
#define MAX_OBJ 32
layout (std140) uniform Scene {
    Object u_Objects[MAX_OBJ];
    Material u_Materials[MAX_OBJ];
};
uniform uint u_NbObj;

struct Ray {
    vec3 org; // Origin
    vec3 dir; // Direction
};
struct HitInfo {
    float dist;
    vec3 pos;
    vec3 normal;
    uint matIdx;
    bool inside;
};

HitInfo intersectSphere(Ray ray, Object o) {
    // Ray - Sphere intersection
    // (x - ox)² + (y - oy)² + (z - oz)² = r² && p = a + bt, t in R
    // <=> (ax + bx*t - ox)² + (ay + by*t - oy)² + (az + bz*t - oz)² = r²
    // <=> (ax - ox)² + 2*t*(ax - ox)*bx + t²*bx² + [...] = r²
    // <=> dot(b, b)*t² + 2*dot(a - o, b)*t + dot(a - o, a - o) - r² = 0
    // => D = (2*dot(a - o, b))² - 4*dot(b, b)*(dot(a - o, a - o) - r²) soit qD = 1/4 * D = dot(a - o, b)² - dot(b, b) * dot(a - o, a - o)
    //    t = - (2*dot(a - o, b) +/- sqrt(D)) / (2 * dot(b, b)) soit t = (dot(a - o, b) +/- sqrt(qD)) / dot(b, b)

    vec3 opos = o.position.xyz;
    float rad = o.scale.z;

    // vec3 opos = vec3(0, 0, 0);
    vec3 rel = ray.org - opos;

    // float a = dot(ray.dir, ray.dir); // = 1.0 because ray is normalized
    float hb = dot(ray.dir, rel);
    float c = dot(rel, rel) - rad * rad;

    float qD = hb*hb - c;
    // float qD = hb*hb - a*c;
    if (qD < 0) return HitInfo(-1, vec3(0), vec3(0), 0, false);

    float hd = sqrt(qD);
    bool inside = hd + hb > 0.0; 
    float dist = inside ? hd - hb : -(hd + hb);
    // float dist /= a;
    vec3 pos = ray.org + ray.dir * dist;
    return HitInfo(dist, pos, normalize(pos - opos), o.matIdx.x, inside);
}


uint PCG_hash(uint i) {
    uint state = i * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}
float rand(inout uint seed) {
    seed = PCG_hash(seed);
    uint usable = seed & ((1u << 23u) - 1u) | (((1u << 7u) - 1u) << 23u);
    return uintBitsToFloat(usable) - 1.0;
}

#define PI 3.1415926535897931
#define TAU 6.2831853071796
vec2 randVec2Disk(inout uint seed) {
    float theta = rand(seed) * TAU;
    float rad = sqrt(rand(seed));
    return vec2(sin(theta), cos(theta)) * rad;
}
vec2 randVec2Disk2(inout uint seed) {
    float theta = rand(seed) * TAU;
    float rad = rand(seed);
    return vec2(sin(theta), cos(theta)) * rad;
}
vec3 randVec3(inout uint seed) {
    float theta = rand(seed) * TAU;
    float z = 2.0 * rand(seed) - 1.0;
    float l = sqrt(1.0 - z*z);
    return vec3(sin(theta)*l, cos(theta)*l, z);
}
vec3 slerp(vec3 a, vec3 b, float t) {
    float alpha = acos(dot(a, b));
    return (a * sin((1.0-t)*alpha) + b * sin(t*alpha)) / sin(alpha);
}
float bokehLength(vec2 org, int sides) {
    vec2 n = normalize(org);
    float ab = TAU / sides;
    float ap = mod(acos(n.x), ab);
    vec2 m = mix(vec2(1, 0), vec2(cos(ab), sin(ab)), ap / ab);
    return length(m);
}

// https://physics.stackexchange.com/questions/435512/snells-law-in-vector-form
vec4 calcRefraction(vec3 i, vec3 n, float mu) {
    float ni = dot(n, i);
    float left = 1.0 - mu*mu * (1.0 - ni*ni);
    return left < 0 ? vec4(0, 0, 0, 0) : vec4(n * sqrt(left) + mu * (i - ni * n), 1);
}

vec3 trace(Ray ray, uint bounce, inout uint seed) {

    vec3 light = vec3(0);
    vec3 absorb = vec3(1);
    HitInfo hitInfo = HitInfo(1000000.0, vec3(0), vec3(0), 0, false);
    float IORstack[MAX_BOUNCES];
    uint IORidx = 1;
    IORstack[0] = 1.0; // Starts in air (presumably)
    IORstack[1] = 1.0;

    for (uint k = 0; k <= bounce; k++) {
        hitInfo.dist = 100000.0;
        bool hit = false;

        for (uint i = 0; i < u_NbObj; i++) {
            HitInfo new = intersectSphere(ray, u_Objects[i]);
            if (0.0 > new.dist || new.dist > hitInfo.dist) continue;
            
            hit = true;
            hitInfo = new;
        }
        if (!hit) {
            float sunMask = pow(max(0, -dot(ray.dir, u_EnvLightDir[SUN_IDX])), 1000.0) * 100.0;
            vec3 sunLight = u_EnvLightCol[SUN_IDX] * sunMask;
            vec3 skyLight = u_EnvAmbiant * (ray.dir.y * 0.5 + 0.5);
            light += (sunLight + skyLight) * absorb * 1.0;
            break;
        }

        Material mat = u_Materials[hitInfo.matIdx];
        if (hitInfo.inside) hitInfo.normal = -hitInfo.normal;

        vec3 roughV = randVec3(seed);

        bool isSpec = mat.specular.w > rand(seed);
        bool isTransmit = !isSpec && mat.transmission > rand(seed);
        if (isTransmit) {
            vec3 n = -normalize(hitInfo.normal + roughV * mat.roughness);
            // float mu = hitInfo.inside ? (mat.IOR / IORstack[IORidx]) : (IORstack[IORidx] / mat.IOR);
            float mu = 1.0;
            if (hitInfo.inside) {
                IORidx--;
                mu = mat.IOR / IORstack[IORidx];
            }
            else {
                IORidx++;
                IORstack[IORidx] = mat.IOR;
                mu = IORstack[IORidx - 1] / IORstack[IORidx];
            }
            vec4 refraction = calcRefraction(ray.dir, n, mu);
            isTransmit = refraction.w > 0;
            isSpec = refraction.w < 1;
            if (isTransmit) ray.dir = refraction.xyz;
        }
        if (!isTransmit) {
            // Keep IOR if isSpec
            vec3 diffuse = normalize(hitInfo.normal + roughV);
            vec3 specular = reflect(ray.dir, hitInfo.normal);
            vec3 metalic = normalize(specular + roughV * mat.roughness);
            ray.dir = mix(diffuse, metalic, float(isSpec));
        }
        ray.org = hitInfo.pos + ray.dir * 0.001;
        
        light += mat.emission.rgb * mat.emission.w * absorb;
        absorb *= isSpec ? mat.specular.rgb : mat.albedo.rgb;
    }

    return light;
}

void main() {
    ivec2 UV = ivec2(gl_GlobalInvocationID.xy);

    if (u_PathIdx >= u_TotalSamples) return;

    uint seed = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * uint(u_ImageSize.x) + u_PathIdx * u_Seed;
    vec2 uv = 2.0 * vec2(UV) / u_ImageSize.xy - 1.0;
    mat3 camRot = transpose(u_CamInvRotMat);

    float focusDisk = 0.25;
    float focusDistance = 15;

    float baseOffsetStrength = 0.001 * 0; 
    vec3 baseDir = normalize(vec3(uv * vec2(u_ImageSize.z, 1.0), u_CamDepth));
    vec3 baseOrg = camRot * baseDir * 0.03 + u_CamPosition;

    Ray ray;
    ray.org = baseOrg;
    ray.dir = camRot * baseDir;
    vec3 result = vec3(0);
    uint traces = u_MaxTracesPerFrame / 16 + 1;
    for (uint i = 0; i < traces; i++) {
        vec3 inter = vec3(0);
        for (uint j = 0; j < 16; j++) {
            vec2 org = randVec2Disk(seed); 
            vec3 offset = vec3(org * (focusDisk * bokehLength(org, 5) + baseOffsetStrength), 0.0);
            ray.org = baseOrg + camRot * offset;
            ray.dir = camRot * normalize(baseDir * focusDistance - offset);
            inter += trace(ray, u_MaxBounces, seed);
        }
        result += inter / 16.0;
    }
    result /= traces;

    if (u_PathIdx > 1) result = mix(imageLoad(Image, UV).rgb, result, 1.0 / float(u_PathIdx + 1));
    imageStore(Image, UV, vec4(result, 1.0));

    // vec3 result = vec3(rand(seed) > 0.999);
    // imageStore(Image, UV, vec4(result, 1.0));
}