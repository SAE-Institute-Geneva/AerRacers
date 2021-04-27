#version 450 core
precision mediump float;

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out vec3 Position;

layout (std140, binding = 0) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

void main()
{
	TexCoords = aPos;
    vec4 pos = proj * mat4(mat3(view)) * vec4(aPos, 1.0);
    gl_Position = pos.xyww; //z == 1
}
