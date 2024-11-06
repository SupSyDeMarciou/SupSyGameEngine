layout (std140) uniform Camera {
    vec4 u_CamData; // NP, FP, W, H
    vec3 u_CamPosition;
    mat3 u_CamInvRotMat;
    mat4 u_CamProjMat;
    mat4 u_CamRenderMat;
};

float linearizeDepth(float depth01) {
    return 2.0 * u_CamData.y * u_CamData.x / ((u_CamData.y - u_CamData.x) * (1.0 - depth01));
}

vec3 screenToWorld(vec2 uv, float z) {
    vec3 pos = vec3((uv * 2.0 - 1.0) * u_CamData.zw / u_CamData.x, 1.0) * z;
    return transpose(u_CamInvRotMat) * pos + u_CamPosition;
}
vec2 worldToScreen(vec3 pos) {
    vec4 t = u_CamRenderMat * vec4(pos, 1);
    return t.xy / t.w * 0.5 + 0.5;
}

vec3 fragmentDirection_nonLinear(vec2 uv) {
    return transpose(u_CamInvRotMat) * (inverse(u_CamProjMat) * vec4((uv * 2.0 - 1.0), 0.0, 1.0)).xyz;
}
vec3 fragmentDirection(vec2 uv) {
    return normalize(fragmentDirection_nonLinear(uv));
}