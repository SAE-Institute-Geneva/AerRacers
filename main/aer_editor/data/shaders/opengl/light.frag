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
uniform uint lightType;

const uint Diffuse  = 0x00000001u;
const uint Specular = 0x00000002u;
const uint Normal   = 0x00000004u;
const uint Emissive = 0x00000008u;

const uint NoLight = 0u;
const uint Point   = 1u;
const uint Sun	   = 2u;

vec3 CalcNoLight()
{
	vec3 result;
    result = texture(material.diffuse, fs1_in.TexCoords).rgb;
    if (bool(usedMaps & Emissive)) result += texture(material.emissive, fs1_in.TexCoords).rgb;
    
    return result;
}

vec3 CalcDirLight(vec3 viewDir)
{
	vec3 normal, lightDir;
	vec3 ambient = dirLight.ambient * texture(material.diffuse, fs1_in.TexCoords).rgb;
	if (bool(usedMaps & Normal))
	{
 		normal = texture(material.normal, fs1_in.TexCoords).rgb;
    	normal = normalize(normal * 2.0 - 1.0);
    	lightDir = normalize(-fs2_in.TangentLightDir);
	}
	else
	{
    	normal = normalize(fs1_in.Normal);
    	lightDir = normalize(-dirLight.direction);
	}

	float diff = max(dot(normal, lightDir), 0.0);
 	vec3 diffuse = dirLight.diffuse * diff * texture(material.diffuse, fs1_in.TexCoords).rgb;

	vec3 halfwayDir = normalize(lightDir + viewDir);
  	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = dirLight.diffuse * dirLight.specular * spec;
 	if (bool(usedMaps & Specular)) specular *= vec3(texture(material.specular, fs1_in.TexCoords).r);

    diffuse *= dirLight.intensity;
    specular *= dirLight.intensity;

    vec3 result = ambient + diffuse + specular;
    if (bool(usedMaps & Emissive))
    {
    	result += texture(material.emissive, fs1_in.TexCoords).rgb;
    }

	return result;
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
 	vec3 diffuse = lights[index].diffuse * diff * texture(material.diffuse, fs1_in.TexCoords).rgb;
    	
	vec3 halfwayDir = normalize(lightDir + viewDir);
  	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = lights[index].diffuse * lights[index].specular * spec;
 	if (bool(usedMaps & Specular)) specular *= vec3(texture(material.specular, fs1_in.TexCoords).r);
 
    float attenuation = clamp(lights[index].radius / distance, 0.0, 1.0) * lights[index].intensity;
    diffuse *= attenuation;
    specular *= attenuation;
    
    vec3 result = diffuse + specular;
    if (bool(usedMaps & Emissive)) 
    {
    	result += texture(material.emissive, fs1_in.TexCoords).rgb;
    }
	
	return result;
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
	}
	else
	{
		lighting = CalcNoLight();
	}

	FragColor = vec4(lighting, 1.0);
}
