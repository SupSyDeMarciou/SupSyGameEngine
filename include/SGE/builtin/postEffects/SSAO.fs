#version 330 core

uniform sampler2D Source;
uniform sampler2D Normal;
uniform sampler2D Depth;

#define NB_RAND_VECT (256)
uniform vec3[NB_RAND_VECT] RandVec;
uniform bool Pass;
uniform float Bias = 0.0;
uniform float Radius = 0.1;

// Camera
uniform vec4 CameraData; // NCP, FCP, W, H
uniform vec3 CameraPos;
uniform mat3 CameraInverseRotation;
uniform mat4 CameraProjection;

in vec2 TexCoords;

out vec4 FragColor;

float linearizeDepth(float depth) {
    return (u_CamData.x * u_CamData.y) / (u_CamData.y - depth * (u_CamData.y - u_CamData.x));
}

vec3 screenToWorld(vec2 uv, float z) {
    uv = 1.0 - uv * 2.0;
    float cz = (z * (CameraData.y + CameraData.x) - 2 * (CameraData.y * CameraData.x)) / (CameraData.y - CameraData.x);
    // return vec3(uv.x * CameraData.z / CameraData.x * cz, uv.y * CameraData.w / CameraData.x * cz, z);
    return vec3(uv.x * CameraData.z / CameraData.x * cz, uv.y * CameraData.w / CameraData.x * cz, z) - CameraInverseRotation * CameraPos;
}

vec2 worldToScreen(vec3 pos) {
    vec4 res = vec4(pos.x * CameraData.x / CameraData.z, pos.y * CameraData.x / CameraData.w, (pos.z * (CameraData.y + CameraData.x) - 2 * (CameraData.y * CameraData.x)) / (CameraData.y - CameraData.x), pos.z);
    return vec2(res.x / res.z, res.y / res.z);
}

void main() {

    vec2 uv = TexCoords;

    if (!Pass) {
        vec3 normal = normalize((texture(Normal, uv).rgb - 0.5) * 2.0);
        vec3 t = vec3(0, 0, 1);
        float dnt = dot(normal, t);

        vec3 tangent = vec3(0, 1, 0);
        if (abs(dnt) < 0.99) tangent = normalize(t - dnt * normal);
        vec3 bitangent = normalize(cross(normal, tangent));
        mat3 TBN = mat3(tangent, bitangent, normal);

        float fragDepth = linearizeDepth(texture(Depth, uv).r);
        vec3 fragPos = screenToWorld(uv, fragDepth);

        // vec3 accum = vec3(0);
        // float occlusion = 0.0;
        // for (int i = 0; i < NB_RAND_VECT; i++) {
            
        //     vec3 rotVec = TBN * RandVec[i];
        //     accum += rotVec;
        //     vec3 samplePos = fragPos + rotVec * Radius;
        //     vec2 newUv = worldToScreen(samplePos);

        //     float rawSampleDepth = texture(Depth, newUv).r;
        //     if (rawSampleDepth >= 0.999875) continue;

        //     float sampleDepth = linearizeDepth(rawSampleDepth);

        //     occlusion +=
        //         smoothstep(0.0, 1.0, 1 - abs(fragPos.z - sampleDepth)) * 
        //         (sampleDepth <= samplePos.z + Bias ? 1.0 : 0.0);

        // }
            
        // FragColor = vec4(vec3(occlusion / NB_RAND_VECT), 1);
        // FragColor = vec4(accum / NB_RAND_VECT, 1);
        // FragColor = vec4(TBN * RandVec[1], 1);
        // FragColor = vec4(normal, 1);
        // FragColor = vec4(vec3(fragDepth), 1);
        FragColor = vec4(fragPos, 1);
    }
    else {
        // float AO = texture(Depth, uv).r;
        // float fact = min(1, max(0, min(1, 1 - AO) * 2.0 - 1.0));
        // FragColor = texture(Source, uv) * fact;
        FragColor = vec4(texture(Depth, uv).rgb, 1.0);
    }
}