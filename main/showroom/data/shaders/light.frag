#version 330 core
precision mediump float;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in Regular
{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
} fs1_in;

in NormalMap
{
    vec3 TangentLightPos;
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
	vec3 diffuseColor;
	vec3 specularColor;
};
uniform Material material;

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

struct SpotLight 
{
    float blend;
    float angle;
    float radius;
};
uniform SpotLight sLight;

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
const uint Spot    = 3u;

vec3 GetDiffuse()
{
	vec3 diffuse;
 	if (bool(usedMaps & Diffuse)) diffuse = texture(material.diffuse, fs1_in.TexCoords).rgb;
 	else diffuse = material.diffuseColor;
 	
    return diffuse;
}

vec3 GetSpecular()
{
 	if (bool(usedMaps & Specular)) return vec3(texture(material.specular, fs1_in.TexCoords).r);
 	
    return material.specularColor;
}

vec3 GetEmissive()
{
    if (bool(usedMaps & Emissive)) 
    {
    	vec3 emissive = texture(material.emissive, fs1_in.TexCoords).rgb;
    	return emissive * 100.0f;
    }
    
    return vec3(0.0);
}

vec3 CalcNoLight()
{
	vec3 result;
 	result = GetDiffuse();
    result += GetEmissive();
    
    return result;
}

vec3 CalcDirLight()
{
	vec3 normal, lightDir, viewDir;
 	vec3 ambient = light.ambient * GetDiffuse();
 	
	if (bool(usedMaps & Normal))
	{
 		normal = texture(material.normal, fs1_in.TexCoords).rgb;
    	normal = normalize(normal * 2.0 - 1.0);
    	lightDir = normalize(-fs2_in.TangentLightDir);
    	viewDir = normalize(fs2_in.TangentViewPos - fs2_in.TangentFragPos);
	}
	else
	{
    	normal = normalize(fs1_in.Normal);
    	lightDir = normalize(-light.direction);
    	viewDir = normalize(viewPos - fs1_in.FragPos);
	}
	
	float diff = max(dot(normal, lightDir), 0.0);
 	vec3 diffuse = light.diffuse * diff * GetDiffuse();
    	
	vec3 halfwayDir = normalize(lightDir + viewDir);
  	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
  	if (spec > 0.25) spec = 1.0;
  	else spec = 0.0;
  	
	vec3 specular = vec3(light.specular) * spec * GetSpecular();
 
    diffuse *= light.intensity;
    specular *= light.intensity;
    
    vec3 result = ambient + diffuse + specular + GetEmissive();
	
	return result;
}

vec3 CalcPointLight()
{
	vec3 normal, lightDir, viewDir;
	float distance;
 	vec3 ambient = light.ambient * GetDiffuse();
	
	if (bool(usedMaps & Normal))
	{
 		normal = texture(material.normal, fs1_in.TexCoords).rgb;
    	normal = normalize(normal * 2.0 - 1.0);
    	lightDir = normalize(fs2_in.TangentLightPos - fs2_in.TangentFragPos);
    	viewDir = normalize(fs2_in.TangentViewPos - fs2_in.TangentFragPos);
    	distance = length(light.position - fs2_in.TangentFragPos);
	}
	else
	{
    	normal = normalize(fs1_in.Normal);
    	lightDir = normalize(light.position - fs1_in.FragPos);
    	viewDir = normalize(viewPos - fs1_in.FragPos);
    	distance = length(light.position - fs1_in.FragPos);
	}
	
	float diff = max(dot(lightDir, normal), 0.0);
 	vec3 diffuse = light.diffuse * diff * GetDiffuse();
    	
	vec3 halfwayDir = normalize(lightDir + viewDir);
  	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = vec3(light.specular) * spec * GetSpecular();
 
    float attenuation = clamp(sLight.radius / distance, 0.0, 1.0) * light.intensity;
    diffuse *= attenuation;
    specular *= attenuation;
    
    vec3 result = ambient + diffuse + specular + GetEmissive();
	
	return result;
}

vec3 CalcSpotLight()
{
	vec3 normal, lightDir, viewDir;
	float distance, theta;
 	vec3 ambient = light.ambient * GetDiffuse();
	
	if (bool(usedMaps & Normal))
	{
    	lightDir = normalize(fs2_in.TangentLightPos - fs2_in.TangentFragPos);
    	theta = dot(lightDir, -fs2_in.TangentLightDir);
    	
    	if(theta > sLight.angle)
    	{
 			normal = texture(material.normal, fs1_in.TexCoords).rgb;
    		normal = normalize(normal * 2.0 - 1.0);
    		viewDir = normalize(fs2_in.TangentViewPos - fs2_in.TangentFragPos);
    		distance = length(light.position - fs2_in.TangentFragPos);
    	}
    	else
    	{
    		return ambient;
    	}
	}
	else
	{
    	lightDir = normalize(light.position - fs1_in.FragPos);
    	theta = dot(lightDir, normalize(-light.direction));
    	
    	if(theta > sLight.angle)
    	{
    		normal = normalize(fs1_in.Normal);
    		viewDir = normalize(viewPos - fs1_in.FragPos);
    		distance = length(light.position - fs1_in.FragPos);
    	}
    	else
    	{
    		return ambient;
    	}
	}
	
	float diff = max(dot(lightDir, normal), 0.0);
 	vec3 diffuse = light.diffuse * diff * GetDiffuse();
    	
	vec3 halfwayDir = normalize(lightDir + viewDir);
  	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = vec3(light.specular) * spec * GetSpecular();
 
    float attenuation = clamp(sLight.radius / distance, 0.0, 1.0) * light.intensity;
    	
	float epsilon   = sLight.blend - sLight.angle;
	float intensity = clamp((theta - sLight.angle) / epsilon, 0.0, 1.0) * attenuation;
    diffuse  *= intensity;
    specular *= intensity;
    
    vec3 result = ambient + diffuse + specular + GetEmissive();
	
	return result;
}

void main()
{
	vec3 result;
	switch (lightType) 
	{
	case NoLight:
	    result = CalcNoLight();
	    break;
	case Point:
	    result = CalcPointLight();
	    break;
	case Sun:
	    result = CalcDirLight();
	    break;
	case Spot:
	    result = CalcSpotLight();
	    break;
	}
	
	float intensity = dot(normalize(fs1_in.Normal), -light.direction);

	if (intensity > 0.95) result = result;
	else if (intensity > 0.5) result = result * 0.6;
	else result = light.ambient * GetDiffuse();
	
	FragColor = vec4(result, 1.0);
	
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
