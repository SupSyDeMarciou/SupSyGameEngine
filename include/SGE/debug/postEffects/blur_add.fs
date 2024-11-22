#version 330 core

uniform int Total;
uniform int Pass;

uniform sampler2D Source;
uniform sampler2D Self;

in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;

void main() {
    vec4 color = texture(Source, TexCoords) * (Pass + 1);
    if (Pass > 0) color += texture(Self, TexCoords);
    if (Pass == Total - 1) color /= Total;
    FragColor = color;
}