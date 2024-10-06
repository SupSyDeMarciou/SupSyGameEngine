#version 330 core

uniform float Gamma;
uniform sampler2D Source;

in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;

void main()
{
    
    vec3 color = texture(Source, TexCoords).rgb;

    color = color / (color + 1); // Tone mapper
    color = pow(color, vec3(1.0 / Gamma)); // Gamma correct

    FragColor = vec4(color, 1.0);
}