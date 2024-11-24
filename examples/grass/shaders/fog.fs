#version 330 core

#include "!render/shaders/environment.glsl"
#include "!render/shaders/camera.glsl"

float orthDepth(float depth) {
    return linearizeDepth(depth) / (u_CamFarPlane - u_CamNearPlane) + u_CamNearPlane;
}

uniform sampler2D Source;
uniform sampler2D Depth;
uniform float u_FogDepth;
uniform float u_Intensity;

// Vertex Shader Output
in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;

void main()
{
    vec3 rayDir = fragmentDirection(TexCoords);
    float depthNonLinear = texture2D(Depth, TexCoords).r;
    vec3 source = texture2D(Source, TexCoords).rgb;
    
    float effect = dot(rayDir, normalize(vec3(rayDir.x, 0, rayDir.z)));
    float depth = linearizeDepth(depthNonLinear);
    float fog = depth / u_FogDepth;
    fog = (1 - exp(-fog*fog) * u_Intensity);

    vec3 color = mix(source, u_EnvAmbiant, min(fog, 1) * effect);
    FragColor = vec4(color, 1.0);
}