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
#include "vk/pipelines/render_pipeline.h"

namespace neko::vk
{
using StageIndex = std::pair<PipelineStage, std::size_t>;

class SubrendererContainer
{
public:
	SubrendererContainer()  = default;
	~SubrendererContainer() = default;

	SubrendererContainer(const SubrendererContainer&) = delete;
	SubrendererContainer& operator=(const SubrendererContainer&) = delete;

	void Destroy() const;

	template<typename T>
	T& Get() const
	{
		static_assert(
			std::is_base_of<RenderPipeline, T>::value, "T has to be a subset of RenderPipeline");

		auto typeId = T::GetSubrendererIndex();
		auto it     = subrenderers_.find(typeId);
		neko_assert(
			it != subrenderers_.end() && it->second, "Error when trying to access subrenderer");

		return static_cast<T&>(*it->second);
	}

	template<typename T, typename... Args>
	void Add(const PipelineStage& stage, std::unique_ptr<T>&& subrenderer)
	{
		static_assert(
			std::is_base_of<RenderPipeline, T>::value, "T has to be a subset of RenderPipeline");

		auto typeId = T::GetSubrendererIndex();
		stages_.insert({StageIndex(stage, subrenderers_.size()), typeId});
		subrenderers_[typeId] = std::move(subrenderer);
	}

	void RenderStage(const PipelineStage& stage, const CommandBuffer& commandBuffer)
	{
		for (const auto& [stageIndex, typeId] : stages_)
		{
			if (stageIndex.first != stage) continue;

			if (auto& subrenderer = subrenderers_[typeId])
				if (subrenderer->IsEnabled()) subrenderer->Render(commandBuffer);
		}
	}

private:
	std::unordered_map<Index, std::unique_ptr<RenderPipeline>> subrenderers_ {};
	std::multimap<StageIndex, int> stages_ {};
};

inline void SubrendererContainer::Destroy() const
{
	for (auto& subrenderer : subrenderers_) subrenderer.second->Destroy();
}
}    // namespace neko::vk
