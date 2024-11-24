#version 330 core

#include "!render/shaders/maths.glsl"
#include "!render/shaders/camera.glsl"

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUv;
layout (location = 4) in vec4 aGrassData; // vec2 pos, float scale, float rot

uniform sampler2D u_DisplacementMap;
uniform float u_Strength;
uniform float u_Size;
uniform int u_Count;

uniform float u_Time;

// Vertex shader output
out VS_OUT {
    vec4 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
    vec2 bigTexUV;
    float randPer;
} vs_out;

void main()
{
    int idx = gl_InstanceID;
    float baseWidth = 1.0;
    float tipWidth = 0.02;

    float c = cos(aGrassData.w), s = sin(aGrassData.w);
    float scaling = mix(baseWidth, tipWidth, aUv.y);
    vec3 normal = vec3(c * aNormal.x - s * aNormal.z, aNormal.y, s * aNormal.x + c * aNormal.z);

    float size = u_Size - 2.0;
    vec2 basePos = aGrassData.xy + vec2(idx % u_Count, idx / u_Count) * (size / float(u_Count));
    vec2 uv = (basePos + 1) / u_Size;
    vec3 noise = texture2D(u_DisplacementMap, uv).rgb;

    vec3 wind = texture2D(u_DisplacementMap, uv + vec2(u_Time * 0.1)).rgb;
    float windFact = pow(wind.x, 3.0);

    normal = normalize(normal - vec3(0, 0.5, 0)) * aUv.y * aUv.y * windFact;
    vec3 bend = vec3(c * aPos.x - s * aPos.z, aPos.y, s * aPos.x + c * aPos.z) * vec3(scaling, aGrassData.z, scaling) + normal;
    vec3 truePos = vec3(basePos.x, 0, basePos.y) + bend + vec3(0, 2.0 * noise.x - 1.0, 0) * u_Strength - vec3(size * 0.5, 0.0, size * 0.5);

    vs_out.position = vec4(truePos, 1);
    vs_out.color = aCol;
    vs_out.normal = normal;
    vs_out.uv = aUv;
    vs_out.bigTexUV = uv;
    uint i = uint(idx);
    vs_out.randPer = rand(i);

    gl_Position = u_CamRenderMat * vs_out.position;
    
}