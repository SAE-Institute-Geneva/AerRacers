#version 450 core
precision mediump float;

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube tex;

void main()
{    
    FragColor = texture(tex, TexCoords);
}
