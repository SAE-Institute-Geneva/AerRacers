#include "vk_engine.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
VkEngine::VkEngine(const FilesystemInterface& filesystem, Configuration* config)
   : SdlEngine(filesystem, *config)
{
	RegisterSystem(drawSystem_);
	RegisterOnEvent(drawSystem_);
}

void VkEngine::Destroy()
{
	drawSystem_.Destroy();
	SdlEngine::Destroy();
}

void VkEngine::GenerateUiFrame()
{
	if (VkResources::Inst->IsImGuiReady())
		drawImGuiAction_.Execute();
}

void VkEngine::SetWindowAndRenderer(Window* window, neko::Renderer* renderer)
{
	window_ = window;

	if (renderer)
	{
		renderer_ = renderer;
		renderer_->SetWindow(window);
		RendererLocator::provide(renderer);
	}
}
}    // namespace neko::vk