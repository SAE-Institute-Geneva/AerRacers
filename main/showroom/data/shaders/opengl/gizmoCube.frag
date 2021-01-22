#version 300 es
precision mediump float;

out vec4 FragColor;

in vec3 FragPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform vec4 color;

void main()
{
    FragColor = color;
}