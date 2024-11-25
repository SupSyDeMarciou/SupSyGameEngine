#version 460 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout (rgba16f, binding = 0) uniform image2D Texture;

uniform int u_Resolution;
uniform float u_Scale;
uniform float u_Detail;
uniform float u_Roughness;
uniform float u_Distortion;

#include "!render/shaders/maths.glsl"

float fractalNoise2D(vec2 p, float scale, float detail, float roughness, float distortion) {

    int iDepth = int(floor(detail));
    
    vec2 pos = p * pow(2.0, float(iDepth)) * scale;
    float sum = noise2D_perlin(2.0 * pos) * fract(detail);
    float total = fract(detail);
    
    for (int i = 0; i < iDepth; i++) {
        sum += noise2D_perlin(pos);
        pos *= 0.5 * mix(1, sum, distortion);
        sum *= roughness;
        total = roughness * (1.0 + total);
    }
    
    return sum / total;
}

void main()
{
    vec2 pos = gl_GlobalInvocationID.xy / float(u_Resolution);

    vec4 color = vec4(
        fractalNoise2D(pos, u_Scale, u_Detail, u_Roughness, u_Distortion) * 0.5 + 0.5,
        fractalNoise2D(pos + vec2(19.929, 10.7567), u_Scale * 4.0, u_Detail * 0.5, u_Roughness * 0.5, u_Distortion * 0.5) * 0.5 + 0.5,
        fractalNoise2D(pos + vec2(2.91, -91.72), u_Scale * 8.0, u_Detail * 0.25, u_Roughness * 0.25, u_Distortion * 0.25) * 0.5 + 0.5,
        1.0
    );

    imageStore(Texture, ivec2(gl_GlobalInvocationID.xy), color);
}