#version 300 es

layout(location = 0) in vec3 aPos;
layout(location = 1) in float linePos;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};
uniform mat4 model;

uniform vec3 endPos;

void main()
{
	if (linePos == 0.0)
	{
		gl_Position = proj * view * model * vec4(aPos, 1.0);
	} 
	else
	{
		gl_Position = proj * view * model * vec4(endPos, 1.0) * vec4(aPos, 1.0);
	}
}