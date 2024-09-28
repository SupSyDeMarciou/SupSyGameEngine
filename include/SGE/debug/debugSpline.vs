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
    vec3 position;
    vec3 p0;
    vec3 p1;
    vec3 p2;
    vec3 color;
    float thickness;
} vs_out;

void main() {
    vec3 maxP = max(max(aSplineP0, aSplineP1), aSplineP2);  
    vec3 minP = min(min(aSplineP0, aSplineP1), aSplineP2);
    vec3 dim = (maxP - minP) + 2.0 * aThickness;
    vec3 pos = (maxP + minP) * 0.5;

    vec3 position = (aPos * dim + pos).xzy;
    vs_out.position = position;
    vs_out.p0 = aSplineP0;
    vs_out.p1 = aSplineP1;
    vs_out.p2 = aSplineP2;
    vs_out.color = aColor;
    vs_out.thickness = aThickness;
    
    gl_Position = vec4(position.x, position.y * u_Ratio, position.z, 1);
}