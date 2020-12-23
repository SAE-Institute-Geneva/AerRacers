//
// Created by frousties on 07/10/2020.
//
#include "vk_engine.h"
#include "vk/graphics.h"
#include "vk/renderers/renderer_editor.h"

int main(int argc, char** argv)
{
	using namespace neko;
    Configuration config;
    config.windowSize = Vec2u(1280, 720);
    config.windowName = "Vulkan Creation Test";

    vk::VkEngine engine(&config);
    sdl::VulkanWindow window;
    engine.SetWindowAndRenderer(&window);
    engine.Init();

    vk::VkRenderer renderer(&window);
    renderer.SetRenderer(std::make_unique<vk::RendererEditor>());
    engine.SetWindowAndRenderer(&window, &renderer);

    engine.EngineLoop();
    return 0;
}