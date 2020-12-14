#pragma once
#include "vk/pipelines/material_pipeline.h"

namespace neko::vk
{
class MaterialPipelineContainer
{
public:
	MaterialPipelineContainer() = default;
	MaterialPipeline& AddMaterial(
			Pipeline::Stage pipelineStage,
			const GraphicsPipelineCreateInfo& pipelineCreate);
	[[nodiscard]] MaterialPipeline& GetMaterial(
			Pipeline::Stage pipelineStage,
			const GraphicsPipelineCreateInfo& pipelineCreate) const;

private:
	std::vector<std::pair<Pipeline::Stage, GraphicsPipelineCreateInfo>> registeredInfos_;
	std::vector<std::unique_ptr<MaterialPipeline>> registeredMaterials_;
};
}
