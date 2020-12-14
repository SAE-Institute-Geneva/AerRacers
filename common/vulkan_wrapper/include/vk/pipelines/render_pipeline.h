#pragma once
#include "vk/pipelines/pipeline.h"

namespace neko::vk
{
class RenderPipeline
{
public:
    explicit RenderPipeline(const Pipeline::Stage stage) : stage_(stage), enabled_(true) {}
    virtual ~RenderPipeline() = default;
	virtual void Destroy() const = 0;

    virtual void OnRender(const CommandBuffer& commandBuffer) = 0;

    [[nodiscard]] const Pipeline::Stage& GetStage() const { return stage_; }

    [[nodiscard]] bool IsEnabled() const { return enabled_; }
    void SetEnabled(const bool& enable) { enabled_ = enable; }

private:
    Pipeline::Stage stage_;
    bool enabled_;
};
}
