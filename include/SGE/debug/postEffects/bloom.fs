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
    vec3 color = texture(Source, uv).rgb;

    if (Pass) {
        vec3 blur = texture(Blured, uv).rgb;
        // color += blur * 8.0 / float(NbIter) * Intensity;
        color += blur * 8.0 * Intensity;
        // color = blur;
    } else {
        float ln = dot(color, vec3(0.2126, 0.7152, 0.0722)); // luminosiy
        float t = max(0.0, ln - Threshold) * 0.125; // Magic constant
        color = mix(vec3(0), color, t);
    }


    FragColor = vec4(color, 1.0);
}