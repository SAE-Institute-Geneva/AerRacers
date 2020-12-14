#include "vk/pipelines/material_pipeline.h"

#include <utility>

#include "vk/graphics.h"

namespace neko::vk
{
MaterialPipeline::MaterialPipeline(
		Pipeline::Stage pipelineStage,
		GraphicsPipelineCreateInfo pipelineCreate)
		: pipelineStage_(pipelineStage),
		  pipelineGraphicsCreate_(std::move(pipelineCreate)),
		  renderStage_(std::neko::nullopt),
		  pipeline_(nullptr) {}

MaterialPipeline& MaterialPipeline::CreateMaterialPipeline(
		Pipeline::Stage pipelineStage,
		const GraphicsPipelineCreateInfo& pipelineCreate)
{
	return VkObjectsLocator::get().AddMaterialPipeline(
			pipelineStage,
			pipelineCreate);
}

bool MaterialPipeline::BindPipeline(const CommandBuffer& commandBuffer)
{
	const auto& renderStage =
			VkObjectsLocator::get().GetRenderStage(pipelineStage_.renderPassId);

	if (!renderStage_ || &renderStage_.value() != &renderStage)
	{
		renderStage_.emplace(renderStage);

		pipeline_ = std::make_unique<GraphicsPipeline>(
				pipelineStage_, pipelineGraphicsCreate_);
	}

	pipeline_->BindPipeline(commandBuffer);
	return true;
}
}