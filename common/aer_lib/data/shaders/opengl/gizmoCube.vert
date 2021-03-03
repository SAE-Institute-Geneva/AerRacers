#version 300 es

layout(location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};
uniform mat4 model;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
}