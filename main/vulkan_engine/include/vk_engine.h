#pragma once
#include "vk_draw_system.h"
#include "sdl_engine/sdl_engine.h"
#include "vk/vulkan_window.h"

namespace neko::vk
{
class VkEngine final : public sdl::SdlEngine
{
public:
    VkEngine() = delete;

    explicit VkEngine(Configuration* config = nullptr) : SdlEngine(config)
    {
        RegisterSystem(drawSystem_);
        RegisterOnEvent(drawSystem_);
    }

	void Destroy() override
    {
		drawSystem_.Destroy();
		SdlEngine::Destroy();
    }

	void GenerateUiFrame() override {}

    void SetWindowAndRenderer(sdl::VulkanWindow* window, Renderer* renderer)
    {
        window_ = window;
        renderer_ = renderer;
        renderer_->SetWindow(window);
        RendererLocator::provide(renderer);
    }
	
private:
	VkDrawSystem drawSystem_;
};
}