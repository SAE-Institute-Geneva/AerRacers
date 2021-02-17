#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D diffuse;

void main()
{
    //FragColor = texture(diffuse, TexCoords);
    FragColor.rgb = vec3(0.8);
    FragColor.a = 1.0;
}