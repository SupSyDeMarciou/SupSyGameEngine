#version 330 core

uniform sampler2D Source;
uniform vec2 OutputSize;
uniform vec2 InputSize;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    // vec2 offset = ((InputSize - OutputSize) / (InputSize * 2));

    // float rescale = OutputSize.y / InputSize.y;

    // // vec2 uv = TexCoords * rescale + vec2(offset.x, 0);
    // vec2 uv = TexCoords * vec2(1.0, rescale);

    vec2 uv = TexCoords;

    vec3 color = texture(Source, uv).rgb;
    FragColor = vec4(color, 1.0);
}