//
// Created by frousties on 07/10/2020.
//
#include "vk_engine.h"
#include "vk/graphics.h"

int main(int argc, char** argv)
{
    neko::Configuration config;
    config.windowSize = neko::Vec2u(1280, 720);
    config.windowName = "Vulkan Creation Test";

    neko::sdl::VulkanWindow window;
    neko::vk::VkRenderer renderer;
    neko::vk::VkEngine engine(&config);

    engine.SetWindowAndRenderer(&window, &renderer);
    renderer.SetWindow(&window);

    engine.Init();
    engine.EngineLoop();
    return 0;
}