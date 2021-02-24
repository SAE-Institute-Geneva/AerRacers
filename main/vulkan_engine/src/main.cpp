//
// Created by frousties on 07/10/2020.
//
#include "vk/graphics.h"
#include "vk/renderers/renderer_editor.h"
#include "vk_engine.h"

int main(int, char**)
{
	using namespace neko;
    Configuration config;
    config.windowSize = Vec2u(1280, 720);
    config.windowName = "Vulkan Creation Test";
	config.dataRootPath = "data/";

	Filesystem filesystem;
    vk::VkEngine engine(filesystem, &config);
    sdl::VulkanWindow window;
    engine.SetWindowAndRenderer(&window, nullptr);
    engine.Init();

    vk::VkRenderer renderer(&window);
    renderer.SetRenderer(std::make_unique<vk::RendererEditor>());
    engine.SetWindowAndRenderer(&window, &renderer);

    engine.EngineLoop();
    return 0;
}