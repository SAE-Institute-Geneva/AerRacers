#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;
layout(location = 5) in mat4 inModelMatrix;
layout(location = 9) in mat3 inNormalMatrix;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out vec3 Normal;

layout(binding = 0) uniform UboScene
{
    mat4 view;
    mat4 proj;
	vec3 viewPos;
} ubo;

void main() 
{
	FragPos = vec3(inModelMatrix * vec4(inPosition, 1.0));
	TexCoords = inTexCoords;
	Normal = normalize(inNormalMatrix * inNormal);

    gl_Position = ubo.proj * ubo.view * inModelMatrix * vec4(inPosition, 1.0);
}