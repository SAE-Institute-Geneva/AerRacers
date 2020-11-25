#include "vk/pipelines/material_pipeline_container.h"

namespace neko::vk
{
MaterialPipeline& MaterialPipelineContainer::AddMaterial(
		Pipeline::Stage pipelineStage,
		const GraphicsPipelineCreateInfo& pipelineCreate)
{
	for (std::size_t i = 0; i < registeredInfos_.size(); i++)
		if(registeredInfos_[i].second == pipelineCreate)
			return *registeredMaterials_[i];

	registeredMaterials_.push_back(
			std::make_unique<MaterialPipeline>(pipelineStage, pipelineCreate));

	registeredInfos_.emplace_back(pipelineStage, pipelineCreate);
	return *registeredMaterials_[registeredMaterials_.size() - 1];
}

MaterialPipeline& MaterialPipelineContainer::GetMaterial(
		Pipeline::Stage pipelineStage,
		const GraphicsPipelineCreateInfo& pipelineCreate) const
{
	auto i = 0;
	for (const auto& infoMaterial : registeredInfos_)
	{
		if (infoMaterial.first == pipelineStage &&
			infoMaterial.second == pipelineCreate)
			return *registeredMaterials_[i];
		i++;
	}

	logDebug("Can't access material!");
}
}