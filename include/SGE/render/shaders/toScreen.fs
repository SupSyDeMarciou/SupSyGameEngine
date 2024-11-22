#version 330 core

uniform sampler2D Source;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 color = texture(Source, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}