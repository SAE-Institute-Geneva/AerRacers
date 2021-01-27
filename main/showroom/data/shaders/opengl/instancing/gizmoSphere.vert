#version 300 es

layout(location = 0) in vec3 aPos;
layout(location = 5) in uint aGizmoType;
layout(location = 6) in vec3 aGizmoPos;
layout(location = 7) in vec3 aParameter;

out vec3 FragPos;
out vec3 Color;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

const uint Cube 	= 0u;
const uint Line   	= 1u;
const uint Sphere	= 2u;

mat4 Model(vec3 position, vec3 scale)
{
    return mat4(
        vec4(scale.x, 0.0,	   0.0,		0.0),
        vec4(0.0,	  scale.y, 0.0,		0.0),
        vec4(0.0,	  0.0,	   scale.z,	0.0),
        vec4(position, 					1.0)
    );
}

void main()
{
	mat4 model = Model(aGizmoPos, aParameter);
	FragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * model * vec4(aPos, 1.0);
} 
