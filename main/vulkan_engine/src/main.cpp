//
// Created by frousties on 07/10/2020.
//
#include "vk_engine.h"
#include "vk/graphics.h"
#include "vk/renderers/renderer_editor.h"

int main(int argc, char** argv)
{
    neko::Configuration config;
    config.windowSize = neko::Vec2u(1280, 720);
    config.windowName = "Vulkan Creation Test";

    neko::vk::VkEngine engine(&config);
    neko::sdl::VulkanWindow window;
    engine.SetWindowAndRenderer(&window);
    engine.Init();

    neko::vk::VkRenderer renderer(&window);
    renderer.SetRenderer(std::make_unique<neko::vk::RendererEditor>());
    engine.SetWindowAndRenderer(&window, &renderer);

    engine.EngineLoop();
    return 0;
}