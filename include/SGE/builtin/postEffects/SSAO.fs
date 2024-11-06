#version 330 core

#include "!rendering/shaders/camera.glsl"

uniform sampler2D Source;
uniform sampler2D Normal;

uniform sampler2D Depth;

#define NB_RAND_VECT (256)
uniform vec3[NB_RAND_VECT] RandVec;
uniform bool Pass;
uniform float Radius = 0.25;
uniform float Intensity = 1.0;

in vec2 TexCoords;

out vec4 FragColor;

void main() {

    vec2 uv = TexCoords;

    if (!Pass) {
        float fragDepth_Raw = texture(Depth, uv).r;
        if (fragDepth_Raw >= 0.999875) {
            FragColor = vec4(0, 0, 0, 1);
            return;
        }
        float fragDepth = linearizeDepth(fragDepth_Raw);
        vec3 fragPos = screenToWorld(uv, fragDepth);

        vec3 normal = normalize(texture(Normal, uv).rgb * 2.0 - 1.0);
        vec3 tangent = vec3(1, 0, 0);
        vec3 t = vec3(0, 0, 1);
        float dnt = dot(normal, t);
        if (abs(dnt) < 1.0) tangent = normalize(dnt * normal - t);
        vec3 bitangent = cross(normal, tangent);
        mat3 TBN = mat3(tangent, bitangent, normal);

        float occlusion = 0.0;
        for (int i = 0; i < NB_RAND_VECT; i++) {
            
            vec3 randVec = RandVec[i];
            vec3 rotVec = TBN * randVec * Radius;
            vec3 samplePos = fragPos + rotVec;
            vec2 sampleUV = worldToScreen(samplePos);

            float sampleDepth_Raw = texture(Depth, sampleUV).r;
            if (sampleDepth_Raw >= 0.999875) continue;

            float sampleDepth = linearizeDepth(sampleDepth_Raw);
            vec3 sampleCamAdjustedPos = u_CamInvRotMat * (samplePos - u_CamPosition);
            float sampleVecLen = Radius;

            occlusion += abs(fragDepth - sampleDepth) < sampleVecLen && sampleCamAdjustedPos.z > sampleDepth ? 1.0 : 0.0;
        }

        FragColor = vec4(vec3(occlusion / NB_RAND_VECT), 1);
    }
    else {
        float AO = texture(Depth, uv).r;
        float fact = pow(1.0 - AO, 2.0 * Intensity);
        FragColor = texture(Source, uv) * fact;
    }
}