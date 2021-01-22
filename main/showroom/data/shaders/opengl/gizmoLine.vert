#version 300 es

layout(location = 0) in vec3 aPos;

out vec3 FragPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};
uniform mat4 model;
uniform vec3 endPos;

void main()
{
	if (gl_VertexID == 0)
	{
    	FragPos = vec3(model * vec4(aPos, 1.0));
		gl_Position = projection * view * model * vec4(aPos, 1.0);
	} 
	else
	{
		FragPos = vec3(model * vec4(endPos, 1.0) * vec4(aPos, 1.0));
		gl_Position = projection * view * model * vec4(endPos, 1.0) * vec4(aPos, 1.0);
	}
}