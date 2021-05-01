#version 450 core
#define MAX_LIGHTS 16
precision mediump float;
out vec4 FragColor;

in Regular
{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
} fs1_in;

in NormalMap
{
	vec3 TangentLightPos[MAX_LIGHTS];
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs2_in;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D emissive;
    float shininess;
    vec3 color;
};
uniform Material material;

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

uniform vec3 viewPos;
uniform uint usedMaps;

const uint Diffuse  = 0x00000001u;
const uint Specular = 0x00000002u;
const uint Normal   = 0x00000004u;
const uint Emissive = 0x00000008u;

vec3 GetDiffuse()
{
	vec3 diffuse = material.color;
	if (bool(usedMaps & Diffuse)) diffuse *= texture(material.diffuse, fs1_in.TexCoords).rgb;

	return diffuse;
}

float GetSpecular()
{
	if (bool(usedMaps & Specular)) return texture(material.specular, fs1_in.TexCoords).r;
	else return 1.0;
}

vec3 GetEmissive()
{
	if (bool(usedMaps & Emissive)) return texture(material.emissive, fs1_in.TexCoords).rgb;
	else return vec3(0.0);
}

vec3 CalcNoLight()
{
    return GetDiffuse() + GetEmissive();
}

vec3 CalcDirLight(vec3 viewDir)
{
	vec3 normal, lightDir;
	vec3 ambient = dirLight.ambient * GetDiffuse();
	if (bool(usedMaps & Normal))
	{
 		normal = texture(material.normal, fs1_in.TexCoords).rgb;
    	normal = normal * 2.0 - 1.0;
    	normal.xy *= 0.75;
    	normal = normalize(normal);
    	lightDir = -fs2_in.TangentLightDir;
	}
	else
	{
    	normal = fs1_in.Normal;
    	lightDir = normalize(-dirLight.direction);
	}

	float diff = max(dot(normal, lightDir), 0.0);
	if (diff > 0.51) diff = 1.0;
	else if (diff > 0.5) diff = smoothstep(0.5, 0.51, diff);
	else diff = smoothstep(0.0, 1.732, length(dirLight.ambient));
	
 	vec3 diffuse = dirLight.diffuse * diff * GetDiffuse();

	vec3 halfwayDir = normalize(lightDir + viewDir);
  	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	if (spec > 0.51) spec = 1.0;
	else spec = 0.0;
	
	vec3 specular = dirLight.diffuse * dirLight.specular * spec * GetSpecular();
    diffuse *= dirLight.intensity;
    specular *= dirLight.intensity;

	return ambient + diffuse + specular;
}

vec3 CalcPointLight(int index, vec3 viewDir)
{
	float distance;
	vec3 normal, lightDir;
	if (bool(usedMaps & Normal))
	{
 		normal = texture(material.normal, fs1_in.TexCoords).rgb;
    	normal = normalize(normal * 2.0 - 1.0);
    	lightDir = normalize(fs2_in.TangentLightPos[index] - fs2_in.TangentFragPos);
    	distance = length(lights[index].position - fs2_in.TangentFragPos);
	}
	else
	{
    	normal = normalize(fs1_in.Normal);
    	lightDir = normalize(lights[index].position - fs1_in.FragPos);
    	distance = length(lights[index].position - fs1_in.FragPos);
	}
	
	float diff = max(dot(lightDir, normal), 0.0);
	if (diff > 0.51) diff = 1.0;
	else if (diff > 0.5) diff = smoothstep(0.5, 0.51, diff);
	else diff = 0.0;
	
 	vec3 diffuse = lights[index].diffuse * diff * GetDiffuse();
    	
	vec3 halfwayDir = normalize(lightDir + viewDir);
  	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	if (spec > 0.51) spec = 1.0;
	else spec = 0.0;
	
	vec3 specular = lights[index].diffuse * lights[index].specular * spec * GetSpecular();
 
    float attenuation = clamp(lights[index].radius / distance, 0.0, 1.0) * lights[index].intensity;
    diffuse *= attenuation;
    specular *= attenuation;

	return diffuse + specular;
}

void main()
{
	vec3 lighting, viewDir;
	if (bool(usedMaps & Normal)) viewDir = normalize(fs2_in.TangentViewPos - fs2_in.TangentFragPos);
	else viewDir = normalize(viewPos - fs1_in.FragPos);
	if (lightNum != 0u)
	{
		lighting += CalcDirLight(viewDir);
		for(int i = 0; i < lightNum; ++i)
		{
			lighting += CalcPointLight(i, viewDir);
		}
		lighting += GetEmissive();
	}
	else
	{
		lighting = CalcNoLight();
	}

	FragColor = vec4(lighting, 1.0);
}
