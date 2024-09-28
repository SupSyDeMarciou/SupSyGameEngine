#version 460 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
// layout (rgba16f, binding = 0) readonly uniform image2D Source;
// layout (rgba16f, binding = 1) writeonly uniform image2D Destination;
layout (rgba16f, binding = 0) uniform image2D Source;
layout (rgba16f, binding = 1) uniform image2D Destination;

uniform bool Pass;

// #define PI 3.141592
// #define INVSQRT2PI 0.3989422804
// float gauss(float x, float invSigma) {
//     float p = x*invSigma;
//     return INVSQRT2PI*invSigma*exp(-p*p * 0.5);
// }

// #define SIZE 32
// #define INV_SIGMA 1.0 / 10.1
// void main() {
//     ivec2 uv = ivec2(gl_GlobalInvocationID.xy);
//     ivec2 e = ivec2(1, 0);

//     if (!Pass) {
//         vec3 base = imageLoad(Source, uv).rgb;
//         vec3 color = base * gauss(0, INV_SIGMA);
//         for (int i = 1; i <= SIZE; i++) {
//             float g = gauss(i, INV_SIGMA);
//             color += imageLoad(Source, uv + e.xy * i).rgb * g;
//             color += imageLoad(Source, uv - e.xy * i).rgb * g;
//         }
//         imageStore(Mid, uv, vec4(color, 1));
//     } else {
//         vec3 base = imageLoad(Mid, uv).rgb;
//         vec3 color = base * gauss(0, INV_SIGMA);
//         for (int i = 1; i <= SIZE; i++) {
//             float g = gauss(i, INV_SIGMA);
//             color += imageLoad(Mid, uv + e.yx * i).rgb * g;
//             color += imageLoad(Mid, uv - e.yx * i).rgb * g;
//         }
//         imageStore(Destination, uv, vec4(color, 1));
//     }
// }

#define SIZE 5
void main() {
    ivec2 uv = ivec2(gl_GlobalInvocationID.xy);
    ivec2 e = ivec2(1, 0);
    float[9] g = {0.1899725, 0.1696098, 0.1207068, 0.0684752, 0.0309638, 0.0111608, 0.0032067, 0.00073442, 0.0001341};

    if (Pass) {
        vec3 base = imageLoad(Destination, uv).rgb;
        vec3 color = base * g[0];
        for (int i = 1; i <= SIZE; i++) {
            color += imageLoad(Destination, uv + e.xy * i).rgb * g[i];
            color += imageLoad(Destination, uv - e.xy * i).rgb * g[i];
        }
        imageStore(Source, uv, vec4(color, 1));
    } else {
        vec3 base = imageLoad(Source, uv).rgb;
        vec3 color = base * g[0];
        for (int i = 1; i <= SIZE; i++) {
            color += imageLoad(Source, uv + e.yx * i).rgb * g[i];
            color += imageLoad(Source, uv - e.yx * i).rgb * g[i];
        }
        imageStore(Destination, uv, vec4(color, 1));
    }
}