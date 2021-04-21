#include "vk/subrenderers/subrenderer_ui.h"

namespace neko::vk
{
SubrendererUi::SubrendererUi(PipelineStage stage) : RenderPipeline(stage), uniformScene_(false) {}

void SubrendererUi::Destroy() const { uniformScene_.Destroy(); }

void SubrendererUi::Render(const CommandBuffer& commandBuffer)
{

}

bool SubrendererUi::CmdRender(
	const CommandBuffer& commandBuffer, ForwardDrawCmd& drawCmd, const Material& mat)
{
	drawCmd.uniformHandle.Push(kUboUiProjHash, uniformScene_);

	// Check if we are in the correct pipeline stage.
	const auto& materialPipeline = mat.GetPipelineMaterial();
	if (materialPipeline.GetStage() != GetStage()) return false;

	// Binds the material pipeline.
	if (!mat.BindPipeline(commandBuffer)) return false;

	const auto& pipeline = materialPipeline.GetPipeline();

	// Updates descriptors.
	drawCmd.descriptorHandle.PushDescriptorData(mat.ExportDescriptorData());
	if (!drawCmd.descriptorHandle.Update(pipeline)) return false;

	// Draws the object.
	drawCmd.descriptorHandle.BindDescriptor(commandBuffer, pipeline);
	return quadUi_.DrawCmd(commandBuffer);
}
}    // namespace neko::vk