#ifdef NEKO_VULKAN
#include "vk/vulkan_window.h"

#include "engine/log.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko::sdl
{
void VulkanWindow::Init()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("VulkanWindowInit");
#endif
	SdlWindow::Init();

#ifdef VALIDATION_LAYERS
	const std::string videoDriver = SDL_GetCurrentVideoDriver();
	logDebug(videoDriver);
#endif
}

void VulkanWindow::Destroy()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("DestroyWindow");
#endif

	SdlWindow::Destroy();
}

void VulkanWindow::MinimizedLoop() const
{
	while (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED) SDL_PumpEvents();
}
}    // namespace neko::sdl
#endif
