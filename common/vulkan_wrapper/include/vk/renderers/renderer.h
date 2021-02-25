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
#include "vk/render_stage/render_stage.h"
#include "vk/subrenderers/subrenderer_container.h"

namespace neko::vk
{
class IRenderer
{
public:
	explicit IRenderer(const bool started = false) : started_(started) {}
	virtual ~IRenderer() = default;

	virtual void Init()          = 0;
	virtual void Start()         = 0;
	virtual void Destroy() const = 0;

	template<typename T, typename... Args>
	void AddSubrenderer(const PipelineStage& pipelineStage, Args&&... args)
	{
		rendererContainer_.Add<T>(
			pipelineStage, std::make_unique<T>(pipelineStage, std::forward<Args>(args)...));
	}

	void SetRenderStage(std::unique_ptr<RenderStage>&& renderStage)
	{
		renderStage_ = std::move(renderStage);
	}

	[[nodiscard]] RenderStage& GetRenderStage() const
	{
		neko_assert(renderStage_, "Error while accessing render stage!");

		return *renderStage_;
	}

	[[nodiscard]] const RenderPass& GetRenderPass() const
	{
		neko_assert(renderStage_, "Error while accessing render pass!");

		return renderStage_->GetRenderPass();
	}

	SubrendererContainer& GetRendererContainer() { return rendererContainer_; }

	[[nodiscard]] bool HasStarted() const { return started_; }

protected:
	bool started_;
	SubrendererContainer rendererContainer_;
	std::unique_ptr<RenderStage> renderStage_;
};
}    // namespace neko::vk