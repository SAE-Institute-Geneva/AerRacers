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

	void GenerateUiFrame() override { /*SdlEngine::GenerateUiFrame();*/ }

    void SetWindowAndRenderer(sdl::VulkanWindow* window, neko::Renderer* renderer = nullptr)
    {
        window_ = window;

        if (renderer)
        {
            renderer_ = renderer;
            renderer_->SetWindow(window);
            RendererLocator::provide(renderer);
        }
    }
	
private:
	VkDrawSystem drawSystem_;
};
}