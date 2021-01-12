#pragma once
#include "mathematics/hash.h"
#include "vk/commands/command_buffer.h"
#include "vk/commands/model_command_buffer.h"
#include "vk/buffers/uniform_handle.h"
#include "vk/pipelines/render_pipeline.h"
#include "vk/models/cube.h"

namespace neko::vk
{
class RenderPipeline;
class UniformHandle;

class SubrendererOpaque final : public RenderPipeline
{
public:
    explicit SubrendererOpaque(Pipeline::Stage stage);
	void Destroy() const override;

    void OnRender(const CommandBuffer& commandBuffer) override;

    constexpr static int GetSubrendererIndex()
    { return static_cast<int>(SubrendererIndex::OPAQUE_S); }

    void SetUniformBlock(const UniformBlock& uniformBlock);

private:
    bool CmdRender(
    		const CommandBuffer& commandBuffer,
    		ForwardDrawCmd& modelDrawCommand,
    		const Mesh& mesh,
    		const Material& mat);
    UniformHandle uniformScene_;

    ModelCommandBuffer& modelCmdBuffer_;
    
    inline static const auto kUboSceneHash = HashString("UboScene");
    inline static const auto kProjectionHash = HashString("proj");
    inline static const auto kViewHash = HashString("view");
	inline static const auto kViewPosHash = HashString("viewPos");

    inline static const auto kUboObjectHash = HashString("UboObject");
    inline static const auto kModelHash = HashString("model");
};
}
