#version 330 core

uniform int NbIter;
uniform bool Pass;
uniform float Threshold = 1.0;
uniform float Intensity = 1.0;

uniform sampler2D Source;
uniform sampler2D Blured;

in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;

void main() {

    vec2 uv = TexCoords;
    vec4 color = texture(Source, uv);

    if (Pass) {
        vec4 blur = texture(Blured, uv);
        color += blur * 8.0 * Intensity;
    } else {
        float ln = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722)); // luminosiy
        float t = max(0.0, ln - Threshold) * 0.125; // Magic constant
        color = mix(vec4(0, 0, 0, color.a), color, t);
    }

    FragColor = color;
}