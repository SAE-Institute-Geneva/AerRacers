#include <gtest/gtest.h>

#ifdef NEKO_VULKAN
#include "utils/file_utility.h"
#include "utils/json_utility.h"
#include "vk/material/diffuse_material.h"

namespace neko::vk
{
//TEST(Materials, TestSaveMaterial)
//{
//	auto image2d = Image2d("aer_racers/textures/stone.png.ktx");
//	auto image = std::optional_const_ref<Image2d>(image2d);
//	DiffuseMaterial material("Test", Color::blue, image);
//	//material.SetShaderPath("aer_racers/shaders/quad_color.aershader");
//
//	ordered_json materialJson = material.ToJson();
//	WriteStringToFile("../../data/aer_racers/materials/test.aermat", materialJson.dump(4));
//
//	const auto& savedFile = LoadJson("../../data/aer_racers/materials/test.aermat");
//	EXPECT_EQ(json(materialJson), savedFile);
//}
//
//TEST(Materials, TestLoadMaterial)
//{
//	const auto& materialJson = LoadJson("../../data/aer_racers/materials/test.aermat");
//
//	DiffuseMaterial material;
//	material.FromJson(materialJson);
//	EXPECT_EQ(material.GetColor(), Color::blue);
//	EXPECT_EQ(material.GetType(), vk::MaterialType::DIFFUSE);
//}
}
#endif 