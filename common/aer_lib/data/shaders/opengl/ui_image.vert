#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aTexCoords;

out vec2 TexCoords;

uniform vec2 slidingCrop;

void main()
{
    TexCoords = vec2(aTexCoords.x * slidingCrop.x, aTexCoords.y * slidingCrop.y);
    gl_Position = vec4(aPos, 1.0);
}