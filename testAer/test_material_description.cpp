/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author : Vincent Dworak
 Co-Author :
 Date : 08.12.20
---------------------------------------------------------- */
#include <gtest/gtest.h>

#ifdef NEKO_GLES3
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "aer_engine.h"
#include "editor/tool/material_description.h"

namespace neko
{
enum class MaterialType : std::uint8_t
{
	DIFFUSE = 0,
	//SKYBOX,
	//TRAIL,
	//PARTICLE
};
class DiffuseMaterial
{
public:
	explicit DiffuseMaterial(
		const std::string& name = "",
		Color4 color = Color::white,
		std::string_view textureAlbedo = "",
		std::string_view textureSpecular = "",
		std::string_view textureNormal = "") :
		name_(name),
		color_(color),
		diffuse_(textureAlbedo),
		specular_(textureSpecular),
		normal_(textureNormal)
		{}

	void SetShaderPath(const std::string& shaderPath) { shaderPath_ = shaderPath; }
	[[nodiscard]] std::string GetShaderPath() const { return shaderPath_; }
	[[nodiscard]] MaterialType GetType() const { return MaterialType::DIFFUSE; }

	void SetColor(const Color4& color)
	{
		color_ = color;
	}
	[[nodiscard]] Color4 GetColor() const { return color_; }

	void SetDiffuse(std::string_view textureDiffuse)
	{
		diffuse_ = textureDiffuse;
	}
	[[nodiscard]] std::string_view GetDiffuse() const { return diffuse_; }

	void SetSpecular(std::string_view textureSpecular)
	{
		specular_ = textureSpecular;
	}
	[[nodiscard]] std::string_view GetSpecular() const { return specular_; }

	void SetNormal(std::string_view textureNormal)
	{
		normal_ = textureNormal;
	}
	[[nodiscard]] std::string_view GetNormal() const { return normal_; }

	[[nodiscard]] std::string_view GetName() const { return name_; }

	void FromJson(const json& materialJson)
	{
		name_ = materialJson["name"];
		shaderPath_ = materialJson["shaderPath"];

		Color4 color;
		color.r = materialJson["color"]["r"];
		color.g = materialJson["color"]["g"];
		color.b = materialJson["color"]["b"];
		color.a = materialJson["color"]["a"];
		SetColor(color);

		if (CheckJsonExists(materialJson, "diffusePath"))
		{
			diffuse_ = materialJson["diffusePath"];
		}

		if (CheckJsonExists(materialJson, "specularPath"))
		{
			specular_ = materialJson["specularPath"];
		}

		if (CheckJsonExists(materialJson, "normalPath"))
		{
			normal_ = materialJson["normalPath"];
		}
	}

private:

	std::string shaderPath_;

	Color4 color_;
	std::string diffuse_;
	std::string specular_;
	std::string normal_;
	std::string name_;
};

TEST(Materials, TestLoadMaterial)
{
	const auto& materialJson = LoadJson("../../data/aer_racers/materials/test.aermat");

	DiffuseMaterial material;
	material.FromJson(materialJson);
	EXPECT_EQ(material.GetColor(), Color::blue);
	EXPECT_EQ(material.GetType(), MaterialType::DIFFUSE);
	EXPECT_EQ(material.GetShaderPath(), "aer_racers/shaders/quad_color.aershader");
	EXPECT_EQ(material.GetName(), "Test");
	EXPECT_EQ(material.GetSpecular(), "");
	EXPECT_EQ(material.GetDiffuse(), "");
	EXPECT_EQ(material.GetNormal(), "");
}
}




#endif