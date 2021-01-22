#version 330 core
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
    vec3 TangentLightPos;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs2_out;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform bool doInverseNormals;
uniform vec3 viewPos;

struct Light 
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    float specular;
    float intensity;
};
uniform Light light;

void main()
{
    vs1_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs1_out.TexCoords = aTexCoords;
    vs1_out.Normal = normalize(normalMatrix * (doInverseNormals ? -aNormal : aNormal));
	
    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(model) * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));
	
    vs2_out.TangentLightPos = TBN * light.position;
    vs2_out.TangentLightDir = TBN * light.direction;
    vs2_out.TangentViewPos  = TBN * viewPos;
    vs2_out.TangentFragPos  = TBN * vs1_out.FragPos;
	
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}