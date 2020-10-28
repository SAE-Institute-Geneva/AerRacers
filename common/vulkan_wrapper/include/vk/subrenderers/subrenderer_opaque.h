#pragma once
#include "mathematics/hash.h"
#include "vk/commands/command_buffer.h"
#include "vk/commands/model_command_buffer.h"
#include "vk/buffers/uniform_handle.h"
#include "vk/pipelines/pipeline.h"
#include "vk/models/quad.h"

namespace neko::vk
{
class RenderPipeline;
class UniformHandle;

class SubrendererOpaque : public RenderPipeline
{
public:
    explicit SubrendererOpaque(Pipeline::Stage stage);

    void OnRender(const CommandBuffer& commandBuffer) override;

    constexpr static int GetSubrendererIndex()
    { return static_cast<int>(SubrendererIndex::OPAQUE); }

private:
    bool CmdRender(const CommandBuffer& commandBuffer, ForwardDrawCmd& modelDrawCommand);
    UniformHandle uniformScene_;

    ModelCommandBuffer& modelCmdBuffer_;

    inline static const auto kProjectionHash = HashString("projection");
    inline static const auto kViewHash = HashString("view");
    inline static const auto kTransformHash = HashString("transform");
    inline static const auto kUboSceneHash = HashString("UboScene");
    inline static const auto kUboObjectHash = HashString("UboObject");

    RenderQuad quad_{Vec3f::zero, Vec2f::one};
};
}