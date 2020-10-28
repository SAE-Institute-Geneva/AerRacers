#pragma once
#include "vk/render_stage/render_stage.h"
#include "vk/subrenderers/subrenderer_container.h"

namespace neko::vk
{
class Renderer {
public:
    explicit Renderer(const bool started = false) : started_(started) { }
    virtual ~Renderer() = default;

    virtual void Init() = 0;
    virtual void Start() = 0;

    template<typename T, typename... Args>
    void AddSubrenderer(const Pipeline::Stage &pipelineStage, Args &&...args)
    {
        rendererContainer_.Add<T>(pipelineStage, std::make_unique<T>(pipelineStage, std::forward<Args>(args)...));
    }

    void AddRenderStage(std::unique_ptr<RenderStage> &&renderStage) {
        renderStages_.emplace_back(std::move(renderStage));
    }

    [[nodiscard]] RenderStage& GetRenderStage(const uint32_t index) const
    {
        neko_assert(!renderStages_.empty() && renderStages_.size() > index, "Error while accession render stages!")

        return *renderStages_.at(index);
    }

    std::vector<std::unique_ptr<RenderStage>>& GetRenderStages() { return renderStages_; }
    SubrendererContainer& GetRendererContainer() { return rendererContainer_; }
    [[nodiscard]] bool HasStarted() const { return started_; }

protected:
    bool started_;
    SubrendererContainer rendererContainer_;
    std::vector<std::unique_ptr<RenderStage>> renderStages_;
};
}