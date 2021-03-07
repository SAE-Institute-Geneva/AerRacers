#include "vk_engine.h"

#include "utils/file_utility.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
VkEngine::VkEngine(const FilesystemInterface& filesystem, Configuration* config)
   : SdlEngine(filesystem, *config)
{
	RegisterSystem(drawSystem_);
	RegisterOnEvent(drawSystem_);
}

void VkEngine::Init()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Init Sdl Engine");
#endif
	logDebug("Current path: " + GetCurrentPath());
	jobSystem_.Init();

	initAction_.Execute();
	inputManager_.Init();
}

void VkEngine::Destroy()
{
	drawSystem_.Destroy();
	SdlEngine::Destroy();
}

void VkEngine::GenerateUiFrame()
{
	if (ImGui::GetCurrentContext())
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