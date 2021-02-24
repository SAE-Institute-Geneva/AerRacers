#include "vk/pipelines/material_pipeline.h"

#include <utility>

#include "vk/vk_resources.h"

namespace neko::vk
{
MaterialPipeline::MaterialPipeline(
	Pipeline::Stage pipelineStage, GraphicsPipelineCreateInfo pipelineCreate)
   : pipelineStage_(pipelineStage),
	 pipelineGraphicsCreate_(std::move(pipelineCreate)),
	 renderStage_(std::nullopt),
	 pipeline_(nullptr)
{}

MaterialPipeline& MaterialPipeline::CreateMaterialPipeline(
	Pipeline::Stage pipelineStage, const GraphicsPipelineCreateInfo& pipelineCreate)
{
	return VkResources::Inst->AddMaterialPipeline(pipelineStage, pipelineCreate);
}

bool MaterialPipeline::BindPipeline(const CommandBuffer& commandBuffer)
{
	RenderStage* renderStage = VkResources::Inst->GetRenderStage();
	if (!renderStage_ || &renderStage_->get() != renderStage)
	{
		renderStage_.emplace(*renderStage);

		pipeline_ = std::make_unique<GraphicsPipeline>(pipelineStage_, pipelineGraphicsCreate_);
	}

	pipeline_->BindPipeline(commandBuffer);
	return true;
}
}    // namespace neko::vk