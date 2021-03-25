#include "vk/pipelines/material_pipeline_container.h"

namespace neko::vk
{
MaterialPipeline& MaterialPipelineContainer::AddMaterial(
	const PipelineStage& pipelineStage, const GraphicsPipelineCreateInfo& pipelineCreate)
{
	for (std::size_t i = 0; i < registeredInfos_.size(); i++)
		if (registeredInfos_[i].second == pipelineCreate) return registeredMaterials_[i];

	registeredMaterials_.emplace_back(pipelineStage, pipelineCreate);
	registeredInfos_.emplace_back(pipelineStage, pipelineCreate);
	return registeredMaterials_.back();
}

std::optional_ref<MaterialPipeline> MaterialPipelineContainer::GetMaterial(
	const PipelineStage& pipelineStage, const GraphicsPipelineCreateInfo& pipelineCreate)
{
	auto i = 0;
	for (const auto& infoMaterial : registeredInfos_)
	{
		if (infoMaterial.first == pipelineStage && infoMaterial.second == pipelineCreate)
			return registeredMaterials_[i];
		i++;
	}

	logDebug("Can't access material!");
	return std::nullopt;
}
}    // namespace neko::vk