#version 330 core

#include "!render/shaders/environment.glsl"
#include "!render/shaders/camera.glsl"

uniform sampler2D u_DisplacementMap;
uniform float u_Strength;
uniform vec4 mColor = vec4(1);

// Vertex shader output
in VS_OUT {
    vec4 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
    vec2 bigTexUV;
    float randPer;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;

void main()
{
    vec3 noise = texture2D(u_DisplacementMap, fs_in.bigTexUV + vec2(0.2, 0.8)).rgb;
    
    vec3 normal = (gl_FrontFacing ? fs_in.normal : -fs_in.normal);
    vec3 viewDir = normalize(u_CamPosition - fs_in.position.xyz);
    vec3 halfwayDir = normalize(u_EnvLightDir[0] + viewDir);

    float fact = fs_in.uv.y * fs_in.uv.y;
    vec3 albedo = mix(vec3(0), mix(mColor.rgb, mColor.rgb * vec3(noise.gb * 2.0 - 1, 0), 0.2), 0.7 * fact + 0.3);
    albedo += vec3(0.5, 0.25, 0) * (fs_in.randPer + 1.0) * 0.1;
    
    float diffuse = max(0, dot(normal, u_EnvLightDir[0]) * 0.85 + 0.15); // Simulate a bit of translucency
    float specular = pow(max(0, dot(normal, halfwayDir)), 10);
    vec3 ambiant = u_EnvAmbiant * albedo * (dot(normal, vec3(0, 1, 0)) * 0.4 + 0.6);

    vec3 color = (diffuse * albedo + specular) * u_EnvLightCol[0] + ambiant;
    FragColor = vec4(color, 1);
    FragNormal = vec4(normal * 0.5 + 0.5, 1);
}