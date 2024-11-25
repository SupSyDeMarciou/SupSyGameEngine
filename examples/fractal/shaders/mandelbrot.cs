#version 460 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform image2D Texture;

uniform uvec2 u_ImageSize;
uniform vec2 u_Pos;
uniform float u_Zoom;


#define MAX_ITER 100
void main()
{
    dvec2 c = (uvec2(gl_GlobalInvocationID.x, u_ImageSize.y-gl_GlobalInvocationID.y) * 2.0 - u_ImageSize) / double(u_ImageSize.y) * 1.5 * u_Zoom + dvec2(u_Pos);
    dvec2 z = dvec2(0);

    int i = 0;
    for (; i < MAX_ITER; i++) {
        z = dvec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
        if (length(z) > 2.0) break;
    }

    float t = i == MAX_ITER ? 0.0 : i / float(MAX_ITER);
    vec3 color = mix(vec3(0, 0, 0), vec3(1, 0.5, 0), t);
    imageStore(Texture, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1));
}