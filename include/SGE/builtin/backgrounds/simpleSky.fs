#version 330 core

#include "!render/shaders/camera.glsl"
#include "!render/shaders/environment.glsl"
uniform uint u_SunIdx = 0u;

// Vertex Shader Output
in vec2 TexCoords;

layout (location = 0) out vec4 o_FragColor;
layout (location = 1) out vec4 o_FragNormal;

void main()
{
    vec3 rayDir = fragmentDirection(TexCoords);
    vec3 sky = (dot(rayDir, vec3(0, 1, 0)) * 0.5 + 0.5) * u_EnvAmbiant;
    vec3 color = sky;

    if (u_EnvNbLights > 0u && u_SunIdx > 0u) {
        float SunRadius = 0.002;
        float sun = max(0, -dot(rayDir, u_EnvLightDir[u_SunIdx - 1u]));
        sun = sun > 1 - SunRadius ? 1.0 : 0.0;

        color += sun * u_EnvLightCol[u_SunIdx - 1u];
    }

    o_FragColor = vec4(color, 1.0);
    o_FragNormal = vec4(rayDir, 1.0);
}