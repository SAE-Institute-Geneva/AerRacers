#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;
uniform vec4 imageColor;

void main()
{
    vec4 sampled = texture(tex, TexCoords);
    FragColor = sampled * imageColor;
}