#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (rgba16f, binding = 0) readonly uniform image2D Source;
layout (rgba16f, binding = 1) writeonly uniform image2D Destination;

void main()
{
    ivec2 uv = ivec2(gl_GlobalInvocationID.xy);
    ivec2 sizeS = imageSize(Source);
    ivec2 sizeD = imageSize(Destination);
    // imageStore(Destination, uv, imageLoad(Source, ivec2(uv * sizeS / vec2(sizeD))));
    imageStore(Destination, uv, imageLoad(Source, uv));
}