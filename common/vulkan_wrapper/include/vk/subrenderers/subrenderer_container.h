#pragma once
#include <map>
#include <memory>
#include <unordered_map>

#include "vk/pipelines/render_pipeline.h"

namespace neko::vk
{
class CommandBuffer;
class SubrendererContainer
{
public:
    SubrendererContainer() = default;
    SubrendererContainer(const SubrendererContainer&) = delete;
    ~SubrendererContainer() = default;
	void Destroy() const;

    SubrendererContainer& operator=(const SubrendererContainer&) = delete;

    template<typename T>
    T &Get() const
    {
        static_assert(HasGetSubrendererIndex<T>::value, "T has to define a function called GetSubrendererIndex");
        static_assert(std::is_base_of<RenderPipeline, T>::value, "T has to be a subset of RenderPipeline");

        auto typeId = T::GetSubrendererIndex();
        auto it = subrenderers_.find(typeId);

        neko_assert(it != subrenderers_.end() && it->second, "Error when trying to access subrenderer");

        return static_cast<T&>(*it->second);
    }

    template<typename T, typename... Args>
    void Add(const Pipeline::Stage &stage, std::unique_ptr<T> &&subrenderer)
    {
        static_assert(HasGetSubrendererIndex<T>::value, "T has to define a function called GetSubrendererIndex");
        static_assert(std::is_base_of<RenderPipeline, T>::value, "T has to be a subset of RenderPipeline");

        auto typeId = T::GetSubrendererIndex();
        stages_.insert({ StageIndex(stage, subrenderers_.size()), typeId });
        subrenderers_[typeId] = std::move(subrenderer);
    }

    void RenderStage(const Pipeline::Stage &stage, const CommandBuffer &commandBuffer)
    {
        for (const auto &[stageIndex, typeId] : stages_)
        {
            if (stageIndex.first != stage) continue;

            if (auto &subrenderer = subrenderers_[typeId])
                if (subrenderer->IsEnabled())
                    subrenderer->OnRender(commandBuffer);
        }
    }

private:
    using StageIndex = std::pair<Pipeline::Stage, std::size_t>;

    std::unordered_map<int, std::unique_ptr<RenderPipeline>> subrenderers_{};
    std::multimap<StageIndex, int> stages_{};
};

inline void SubrendererContainer::Destroy() const
{
	for (auto& subrenderer : subrenderers_)
	{
		subrenderer.second->Destroy();
	}
}
}
