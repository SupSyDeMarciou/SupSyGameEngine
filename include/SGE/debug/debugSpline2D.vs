#version 330

uniform float u_Ratio;

// Vertex
layout (location = 0) in vec3 aPos;
layout (location = 4) in vec3 aSplineP0;
layout (location = 5) in vec3 aSplineP1;
layout (location = 6) in vec3 aSplineP2;
layout (location = 7) in vec3 aColor;
layout (location = 8) in float aThickness;

out VS_OUT {
    vec2 position;
    vec2 p0;
    vec2 p1;
    vec2 p2;

    vec3 color;
    float thickness;
} vs_out;

void main() {
    
    vs_out.p0 = aSplineP0.xy;
    vs_out.p1 = aSplineP1.xy;
    vs_out.p2 = aSplineP2.xy;

    vec2 maxP = max(max(vs_out.p0, vs_out.p1), vs_out.p2);  
    vec2 minP = min(min(vs_out.p0, vs_out.p1), vs_out.p2);
    vec2 dim = (maxP - minP) + 2.0 * aThickness;
    vec2 pos = (maxP + minP) * 0.5;

    vec2 position = aPos.xy * dim + pos;
    vs_out.position = position;
    vs_out.color = aColor;
    vs_out.thickness = aThickness;
    
    gl_Position = vec4(position.x, position.y * u_Ratio, 0, 1);
}