#include "vk/renderers/renderer_editor.h"
#include "vk/subrenderers/subrenderer_opaque.h"

namespace neko::vk
{
void RendererEditor::Init()
{
    std::vector<Attachment> renderPassAttachment1 =
    {
            Attachment(0, "color",     Attachment::Type::SWAPCHAIN, false, VK_FORMAT_B8G8R8A8_SRGB),
            Attachment(1, "depth",     Attachment::Type::DEPTH,     false),
            /*Attachment(0, "depth",     Attachment::Type::DEPTH,     false),
            Attachment(1, "swapchain", Attachment::Type::SWAPCHAIN, false, VK_FORMAT_R8G8B8A8_UNORM),
            Attachment(2, "position",  Attachment::Type::IMAGE,     false, VK_FORMAT_R16G16B16A16_SFLOAT),
            Attachment(3, "diffuse",   Attachment::Type::IMAGE,     false, VK_FORMAT_R8G8B8A8_UNORM),
            Attachment(4, "normal",    Attachment::Type::IMAGE,     false, VK_FORMAT_R16G16B16A16_SFLOAT),
            Attachment(5, "material",  Attachment::Type::IMAGE,     false, VK_FORMAT_R8G8B8A8_UNORM),
            Attachment(6, "resolved",  Attachment::Type::IMAGE,     false, VK_FORMAT_R16G16B16A16_SFLOAT),
            Attachment(7, "bright",    Attachment::Type::IMAGE,     false, VK_FORMAT_R16G16B16A16_SFLOAT)*/
    };

    std::vector<SubpassType> renderPassSubpasses1 =
    {
            SubpassType(0, {0, 1}), //Geometry pass
            //SubpassType(0, {0, 2, 3, 4, 5}), //Geometry pass
            //SubpassType(1, {0, 6, 7}), //Deferred pass
            //SubpassType(2, {0, 1}) //Post process pass
    };

    AddRenderStage(std::make_unique<RenderStage>(renderPassAttachment1, renderPassSubpasses1));
}

void RendererEditor::Start()
{
    started_ = true;

    AddSubrenderer<SubrendererOpaque>(Pipeline::Stage(0, 0));
    /*AddSubrenderer<SubrendererDeferred>(Pipeline::Stage(0, 1));
    AddSubrenderer<SubrendererParticles>(Pipeline::Stage(0, 1));
    AddSubrenderer<SubrendererTransparent>(Pipeline::Stage(0, 1));*/

    //AddSubrenderer<PipelineBlur>(Pipeline::Stage(0, 2), 1.8f);
    /*AddSubrenderer<FilterRipple>(Pipeline::Stage(0, 2));
    AddSubrenderer<FilterFxaa>(Pipeline::Stage(0, 2));*/
    /*AddSubrenderer<FilterDefault>(Pipeline::Stage(0, 2), true);
    AddSubrenderer<SubrendererGizmo>(Pipeline::Stage(0, 2));
    AddSubrenderer<SubrendererImgui>(Pipeline::Stage(0, 2));*/
}

void RendererEditor::Destroy() const
{
	rendererContainer_.Destroy();
	for (const auto& renderStage : renderStages_)
	{
		renderStage->Destroy();
	}
}
}
