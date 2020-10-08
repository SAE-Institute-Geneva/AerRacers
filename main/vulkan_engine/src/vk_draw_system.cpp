#include "vk_draw_system.h"

namespace neko::vk
{
VkDrawSystem::VkDrawSystem() : renderer_(RendererLocator::get())
{
}

void VkDrawSystem::Init()
{
}

void VkDrawSystem::Update(seconds dt)
{
	RendererLocator::get().Render(this);
}

void VkDrawSystem::Render()
{
}

void VkDrawSystem::Destroy()
{
}

void VkDrawSystem::OnEvent(const SDL_Event& event)
{
    /*camera_.OnEvent(event);

    switch (event.type)
    {
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
            	auto& config = BasicEngine::GetInstance()->config;
                config.windowSize.x = event.window.data1;
                config.windowSize.y = event.window.data2;
                std::cout << "Resizing window...\n";
                isFramebufferResized_ = true;
            }
            break;
        default:
            break;
    }*/
}
}
