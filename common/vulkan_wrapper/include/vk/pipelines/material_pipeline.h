#pragma once
#include "optional.hpp"

#include "vk/pipelines/pipeline.h"
#include "vk/pipelines/graphics_pipeline.h"

/*
namespace neko::vk
{
class MaterialPipeline
{
public:
    MaterialPipeline(Pipeline::Stage pipelineStage,
                     const GraphicsPipelineCreateInfo& pipelineCreate);
    ~MaterialPipeline() = default;

    bool BindPipeline(const CommandBuffer& commandBuffer);
    [[nodiscard]] Pipeline::Stage GetStage() const { return pipelineStage_; }
    [[nodiscard]] const GraphicsPipelineCreateInfo& GetGraphicsPipeline() const { return pipelineGraphicsCreate_; }
    [[nodiscard]] const GraphicsPipeline& GetPipeline() const { return *pipeline_; }

private:
    Pipeline::Stage pipelineStage_;
    GraphicsPipelineCreateInfo pipelineGraphicsCreate_;
    std::neko::optional<const RenderStage&> renderStage_ = std::neko::nullopt;
    std::unique_ptr<GraphicsPipeline> pipeline_ = nullptr;
};

static MaterialPipeline& CreateMaterialPipeline(
        Pipeline::Stage pipelineStage,
        const GraphicsPipelineCreateInfo& pipelineCreate);
}*/
