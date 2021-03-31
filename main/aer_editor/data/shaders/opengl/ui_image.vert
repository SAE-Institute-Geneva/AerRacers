#version 450 core
layout(location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform vec2 slidingCrop;

void main()
{
    TexCoords = vertex.zw * slidingCrop;
    gl_Position = vec4(vertex.xy, 1.0);
}