#include <gtest/gtest.h>

#ifdef NEKO_VULKAN
#include "mathematics/hash.h"
#include "mathematics/matrix.h"
#include "utilities/file_utility.h"
#include "utilities/json_utility.h"
#include "vk/shaders/shader.h"

namespace neko::vk
{
TEST(Materials, TestSaveShader)
{
	Shader shader;
	shader.SetStagePaths({"../../data/aer_racers/shaders/vulkan/quad.vert.spv",
	                      "../../data/aer_racers/shaders/vulkan/quad.frag.spv"});
	shader.AddAttribute(Attribute("inPosition", 0, sizeof(Vec3f), Attribute::Type::VEC3F));
	shader.AddAttribute(Attribute("inNormal", 1, sizeof(Vec3f), Attribute::Type::VEC3F));
	shader.AddAttribute(Attribute("inTexCoords", 2, sizeof(Vec2f), Attribute::Type::VEC2F));

	UniformBlock ubo("UboScene", 0, sizeof(Mat4f) * 2, VK_SHADER_STAGE_VERTEX_BIT);
	ubo.AddUniform(Uniform("view", 0, 0, sizeof(Mat4f), VK_SHADER_STAGE_VERTEX_BIT));
	ubo.AddUniform(Uniform("proj", 0, sizeof(Mat4f), sizeof(Mat4f), VK_SHADER_STAGE_VERTEX_BIT));

	UniformBlock object("UboObject", 1, sizeof(Mat4f) + sizeof(Vec4f), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	object.AddUniform(Uniform("model", 0, 0, sizeof(Mat4f), VK_SHADER_STAGE_VERTEX_BIT));
	object.AddUniform(Uniform("color", 0, sizeof(Mat4f), sizeof(Vec4f), VK_SHADER_STAGE_FRAGMENT_BIT));

	shader.AddUniformBlock(ubo);
	shader.AddUniformBlock(object);

	shader.AddUniform(Uniform("diffuse", 2, Uniform::Type::SAMPLER_2D, VK_SHADER_STAGE_FRAGMENT_BIT));

	ordered_json shaderJson = shader.ToJson();
	WriteStringToFile("../../data/aer_racers/shaders/quad_color.aershader", shaderJson.dump(4));

	const auto& savedFile = LoadJson("../../data/aer_racers/shaders/quad_color.aershader");
	EXPECT_EQ(json(shaderJson), savedFile);
}

TEST(Materials, TestLoadShader)
{
	const auto& shaderJson = LoadJson("../../data/aer_racers/shaders/quad_color.aershader");

	Shader shader;
	shader.FromJson(shaderJson);
	EXPECT_EQ(shader.GetStagePaths().size(), 2);
	EXPECT_EQ(shader.GetAttributes().size(), 3);
	EXPECT_EQ(shader.GetUniformBlocks().size(), 2);
}
}
#endif