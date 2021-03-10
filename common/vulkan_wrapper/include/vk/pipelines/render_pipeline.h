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
#include "vk/pipelines/pipeline.h"

namespace neko::vk
{
class RenderPipeline
{
public:
	explicit RenderPipeline(const PipelineStage& stage) : stage_(stage), enabled_(true) {}
	virtual ~RenderPipeline() = default;

	virtual void Destroy() const = 0;

	virtual void Render(const CommandBuffer& commandBuffer) = 0;

	[[nodiscard]] const PipelineStage& GetStage() const { return stage_; }

	void SetEnabled(const bool enable) { enabled_ = enable; }

	[[nodiscard]] bool IsEnabled() const { return enabled_; }

private:
	PipelineStage stage_;
	bool enabled_;
};
}    // namespace neko::vk
