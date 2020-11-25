#include <gtest/gtest.h>

#include "utilities/file_utility.h"
#include "utilities/json_utility.h"
#include "vk/material/diffuse_material.h"

namespace neko::vk
{
TEST(Materials, TestSaveMaterial)
{
	DiffuseMaterial material("Test", Color::blue);

	ordered_json materialJson = material.ToJson();
	WriteStringToFile("../../data/aer_racers/materials/test.aermat", materialJson.dump(4));

	const auto& savedFile = LoadJson("../../data/aer_racers/materials/test.aermat");
	EXPECT_EQ(json(materialJson), savedFile);
}

TEST(Materials, TestLoadMaterial)
{
	const auto& materialJson = LoadJson("../../data/aer_racers/materials/test.aermat");

	DiffuseMaterial material;
	material.FromJson(materialJson);
	EXPECT_EQ(material.GetColor(), Color::blue);
	EXPECT_EQ(material.GetType(), vk::MaterialType::DIFFUSE);
}
}