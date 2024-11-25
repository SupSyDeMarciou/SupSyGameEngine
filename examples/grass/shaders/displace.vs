#version 330 core

#include "!render/shaders/camera.glsl"
#include "!render/shaders/object.glsl"

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUv;

uniform sampler2D u_DisplacementMap;
uniform float u_Strength;

vec3 surfaceNormal(vec2 uv) {
    vec2 e = vec2(0.001, 0);
    
    float z1 = (texture2D(u_DisplacementMap, uv + e.xy).x - texture2D(u_DisplacementMap, uv - e.xy).x) * u_Strength;
    float z2 = (texture2D(u_DisplacementMap, uv + e.yx).x - texture2D(u_DisplacementMap, uv - e.yx).x) * u_Strength;
    vec3 v1 = vec3(2 * e.xy, z1) / u_ObjScale;
    vec3 v2 = vec3(2 * e.yx, z2) / u_ObjScale;
    return normalize(cross(v1, v2));
}


// Vertex shader output
out VS_OUT {
    vec4 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
} vs_out;

void main()
{
    vec3 displace = texture2D(u_DisplacementMap, aUv).rgb;
    vec3 originalNormals = normalize(u_ObjTransformNormalMat * aNormal);
    vs_out.position = u_ObjTransformMat * vec4(aPos, 1) + vec4(vec3(0, 2 * displace.x - 1, 0) * u_Strength, 0);
    vs_out.color = aCol;
    vs_out.normal = normalize(u_ObjTransformNormalMat * normalize(surfaceNormal(aUv)));
    vs_out.uv = aUv;

    gl_Position = u_CamRenderMat * vs_out.position;
}