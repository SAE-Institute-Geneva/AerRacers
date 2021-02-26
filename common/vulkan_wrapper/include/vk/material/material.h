#pragma once
/* ----------------------------------------------------
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

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/material/material_export_data.h"
#include "vk/pipelines/material_pipeline.h"

namespace neko::vk
{
enum class MaterialType : std::uint8_t
{
	DIFFUSE = 0,
	//SKYBOX,
	//TRAIL,
	//PARTICLE
};

class Material
{
public:
	using PushDataContainer = std::vector<std::pair<StringHash, MaterialExportData>>;

	enum class RenderMode : std::uint8_t
	{
		VK_OPAQUE,
		VK_TRANSPARENT
	};

	Material() = default;
	explicit Material(std::string_view name) : name_(name) {}
	virtual ~Material() = default;

	virtual void CreatePipeline(const VertexInput& vertexInput) = 0;
	[[nodiscard]] bool BindPipeline(const CommandBuffer& commandBuffer) const
	{
		if (pipelineMaterial_) return pipelineMaterial_->get().BindPipeline(commandBuffer);
		else return false;
	}

	[[nodiscard]] const PushDataContainer& ExportUniformData() const { return uniformData_; }
	[[nodiscard]] const PushDataContainer& ExportDescriptorData() const { return descriptorData_; }
	[[nodiscard]] const MaterialPipeline& GetPipelineMaterial() const { return *pipelineMaterial_; }

	[[nodiscard]] virtual ordered_json ToJson() const = 0;
	virtual void FromJson(const json& materialJson)   = 0;

	virtual void SetRenderMode(RenderMode renderMode)      = 0;
	[[nodiscard]] virtual MaterialType GetType() const     = 0;
	[[nodiscard]] virtual RenderMode GetRenderMode() const = 0;

	[[nodiscard]] std::string_view GetName() const { return name_; }

protected:
	std::optional_ref<MaterialPipeline> pipelineMaterial_ = std::nullopt;

	std::string name_;

	PushDataContainer uniformData_;
	PushDataContainer descriptorData_;

	RenderMode renderMode_ = RenderMode::VK_OPAQUE;
};
}    // namespace neko::vk