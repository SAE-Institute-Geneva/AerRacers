#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoords;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};
uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}