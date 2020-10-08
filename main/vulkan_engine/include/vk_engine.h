#pragma once
#include <cassert>
#include "sdl_engine/sdl_engine.h"
#include "vk/vulkan_include.h"
#include "vk/vulkan_window.h"

namespace neko::vk
{
class VkEngine : public sdl::SdlEngine
{
public:
    VkEngine() = delete;

    explicit VkEngine(Configuration* config = nullptr) : sdl::SdlEngine(config) {}

    void ManageEvent() override {}

    void GenerateUiFrame() override {}

    void SetWindowAndRenderer(sdl::VulkanWindow* window, Renderer* renderer)
    {
        window_ = window;
        renderer_ = renderer;
        renderer_->SetWindow(window);
        RendererLocator::provide(renderer);
    }
private:
};
}