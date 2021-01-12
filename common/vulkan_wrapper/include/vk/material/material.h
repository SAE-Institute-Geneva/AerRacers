#pragma once
#include <utility>

#include "optional.hpp"

#include "vk/pipelines/material_pipeline.h"
#include "vk/material/material_export_data.h"

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
	using PushDataContainer = std::vector<std::pair<XXH64_hash_t, MaterialExportData>>;

    enum class RenderMode : std::uint8_t
    {
        OPAQUE,
        TRANSPARENT
    };

    explicit Material() = default;
    explicit Material(std::string name) : name_(std::move(name)) {}
    virtual ~Material() = default;

	virtual void CreatePipeline(const VertexInput& vertexInput) = 0;
	[[nodiscard]] bool BindPipeline(const CommandBuffer& commandBuffer) const
	{
		return pipelineMaterial_->BindPipeline(commandBuffer);
	}

	[[nodiscard]] const PushDataContainer& ExportUniformData() const { return uniformData_; }
	[[nodiscard]] const PushDataContainer& ExportDescriptorData() const { return descriptorData_; }
	[[nodiscard]] const MaterialPipeline& GetPipelineMaterial() const { return *pipelineMaterial_; }

	[[nodiscard]] virtual ordered_json ToJson() const = 0;
	virtual void FromJson(const json& materialJson) = 0;

	virtual void SetRenderMode(RenderMode renderMode) = 0;
	[[nodiscard]] virtual MaterialType GetType() const = 0;
	[[nodiscard]] virtual RenderMode GetRenderMode() const = 0;

	[[nodiscard]] std::string GetName() const { return name_; }

protected:
	std::neko::optional<MaterialPipeline&> pipelineMaterial_ = std::neko::nullopt;

	std::string name_;

	PushDataContainer uniformData_;
	PushDataContainer descriptorData_;

	RenderMode renderMode_ = RenderMode::OPAQUE;
};
}