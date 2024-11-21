#version 330

uniform bool u_RenderDots;

in VS_OUT {
    vec2 position;
    vec2 p0;
    vec2 p1;
    vec2 p2;

    vec3 color;
    float thickness;
} fs_in;

out vec4 FragColor;

float dot2(vec2 a) { return dot(a, a); }
float dot2(vec3 a) { return dot(a, a); }

vec2 spline(float t, vec2 p0, vec2 p1, vec2 p2) {
    return (p2 - 2.0 * p1 + p0) * t*t + 2.0 * (p1 - p0) * t + p0;
}

float findTmin(vec2 v, vec2 p0, vec2 p1, vec2 p2) {

    float t = 0.5;
    float e = 0.1;
    float d0 = dot2(spline(t, p0, p1, p2) - v);
    for (int i = 0; i < 15; i++) {
        float d1 = dot2(spline(t + e, p0, p1, p2) - v);
        float d2 = dot2(spline(t - e, p0, p1, p2) - v);
        
        if (d1 > d0 && d2 > d0) e*=0.1;
        
        if (d1 < d2) {
            t += e;
            d0 = d1;
        }
        else {
            t -= e;
            d0 = d2;
        }
    }
    
    if (t >= 1.0) return 1.0;
    if (t <= 0.0) return 0.0;
    return t;
}

void main() {

    float e = fs_in.thickness;
    if (e <= 0.0) discard;
    vec3 color = fs_in.color;
    
    vec2 uv = fs_in.position;
    vec2 p0 = fs_in.p0;
    vec2 p1 = fs_in.p1;
    vec2 p2 = fs_in.p2;

    float dist = 1.0e37;
    if (u_RenderDots) dist = min(length(p0 - uv), min(length(p1 - uv), length(p2 - uv)));

    if (dist > e) {
        float t = findTmin(uv, p0, p1, p2);
        dist = length(spline(t, p0, p1, p2) - uv);
        if (dist < e * 0.66) color *= 1.0;
        // if (dist < e * 0.66) color *= (t < 0.01 ? 0.5 : 1.0);
        else discard;
    }
    else color *= 0.5;

    FragColor = vec4(color, 1);
}