#version 300 es
precision highp float;

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

void main()
{
    gl_Position = proj * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}