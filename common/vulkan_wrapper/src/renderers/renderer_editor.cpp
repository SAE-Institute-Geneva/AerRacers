#include "vk/renderers/renderer_editor.h"

#include "vk/subrenderers/subrenderer_opaque.h"

namespace neko::vk
{
void RendererEditor::Init()
{
	std::vector<Attachment> renderPassAttachments = {
		{0, "color", Attachment::Type::SWAPCHAIN, false},
		{1, "depth", Attachment::Type::DEPTH, false},
	};

	std::vector<SubpassType> renderPassSubpasses = {
		{0, {0, 1}},    //Geometry pass
		//{1, {0, 1}}     //Post process pass
	};

	SetRenderStage(std::make_unique<RenderStage>(renderPassAttachments, renderPassSubpasses));
}

void RendererEditor::Start()
{
	started_ = true;

	AddSubrenderer<SubrendererOpaque>({0, 0});
	/*AddSubrenderer<SubrendererDeferred>({0, 1});
	AddSubrenderer<SubrendererParticles>({0, 1});
	AddSubrenderer<SubrendererTransparent>({0, 1});

	AddSubrenderer<PipelineBlur>({0, 2}, 1.8f);
	AddSubrenderer<FilterRipple>({0, 2});
	AddSubrenderer<FilterFxaa>({0, 2});
	AddSubrenderer<FilterDefault>({0, 2}, true);
	AddSubrenderer<SubrendererGizmo>({0, 2});*/
}

void RendererEditor::Destroy() const
{
	rendererContainer_.Destroy();
	renderStage_->Destroy();
}
}    // namespace neko::vk
