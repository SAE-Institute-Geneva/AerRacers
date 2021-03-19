#version 450 core
#define MAX_LIGHTS 16
precision mediump float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out Regular
{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
} vs1_out;

out NormalMap 
{
    vec3 TangentLightPos[MAX_LIGHTS];
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs2_out;

layout (std140, binding = 0) uniform Matrices
{
    mat4 proj;
    mat4 view;
};
uniform mat4 model;

uniform mat3 normalMatrix;
uniform vec3 viewPos;

struct Light
{
    vec3 position;
    vec3 diffuse;

    float specular;
    float intensity;
    float radius;
};

struct DirLight
{
    vec3 diffuse;
    vec3 ambient;

    vec3 direction;

    float specular;
    float intensity;
};

layout (std140, binding = 1) uniform Lights
{
    uint lightNum;
    DirLight dirLight;
    Light lights[MAX_LIGHTS];
};

void main()
{
    vs1_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs1_out.TexCoords = aTexCoords;
    vs1_out.Normal = normalize(mat3(model) * aNormal);
	
    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(model) * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    for(int i = 0; i < lightNum; ++i)
        vs2_out.TangentLightPos[i] = TBN * lights[i].position;

    vs2_out.TangentLightDir = normalize(TBN * dirLight.direction);
    vs2_out.TangentViewPos  = TBN * viewPos;
    vs2_out.TangentFragPos  = TBN * vs1_out.FragPos;
	
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}