#version 300 es

layout(location = 0) in vec3 aPos;
layout(location = 1) in float linePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 endPos;

void main()
{
	if (linePos == 0.0)
	{
		gl_Position = projection * view * model * vec4(aPos, 1.0);
	} else
	{
		gl_Position = projection * view * model * vec4(endPos, 1.0) * vec4(aPos, 1.0);
	}
}