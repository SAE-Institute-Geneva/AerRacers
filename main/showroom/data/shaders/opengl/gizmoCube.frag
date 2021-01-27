#version 300 es
precision mediump float;

out vec4 FragColor;

in vec3 FragPos;

uniform vec4 color;

void main()
{
    FragColor = color;
}