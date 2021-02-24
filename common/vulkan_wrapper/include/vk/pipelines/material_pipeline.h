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
#include "utils/std_utility.h"

#include "vk/pipelines/graphics_pipeline.h"
#include "vk/render_stage/render_stage.h"

namespace neko::vk
{
class MaterialPipeline
{
public:
	MaterialPipeline(Pipeline::Stage pipelineStage, GraphicsPipelineCreateInfo pipelineCreate);
	~MaterialPipeline() = default;

	static MaterialPipeline& CreateMaterialPipeline(
		Pipeline::Stage pipelineStage, const GraphicsPipelineCreateInfo& pipelineCreate);

	bool BindPipeline(const CommandBuffer& commandBuffer);
	[[nodiscard]] Pipeline::Stage GetStage() const { return pipelineStage_; }
	[[nodiscard]] const GraphicsPipeline& GetPipeline() const { return *pipeline_; }

private:
	Pipeline::Stage pipelineStage_;
	GraphicsPipelineCreateInfo pipelineGraphicsCreate_;
	std::optional_const_ref<RenderStage> renderStage_ = std::nullopt;
	std::unique_ptr<GraphicsPipeline> pipeline_       = nullptr;
};
}    // namespace neko::vk
