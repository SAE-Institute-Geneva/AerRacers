#version 450 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec2 TexCoords;

layout (std140, binding = 2) uniform UiProj
{
    mat4 proj;
};

uniform vec2 slidingCrop;

void main()
{
    TexCoords = vertex.zw * slidingCrop;
    gl_Position = proj * vec4(vertex.xy, 0.0, 1.0);
}