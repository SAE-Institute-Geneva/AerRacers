#version 300 es
precision mediump float;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Color;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

const float maxDist = 100.0;
const float fadeThreshold = 80.0;

void main()
{
	vec3 vector = FragPos - vec3(viewPos.x, 0, viewPos.z);
	float magnitude = length(vector);
	
	float factor = 1.0;
	if (magnitude >= fadeThreshold) factor = 1.0 - ((magnitude - fadeThreshold) / (maxDist - fadeThreshold));
	
    FragColor.rgb = Color;
    FragColor.a = factor;
} 
