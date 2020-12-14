#include "vk_draw_system.h"

namespace neko::vk
{
VkDrawSystem::VkDrawSystem() : renderer_(RendererLocator::get())
{
}

void VkDrawSystem::Init()
{
    camera_.Init();
    camera_.reverseDirection = Vec3f::back;
    CameraLocator::provide(&camera_);
}

void VkDrawSystem::Update(seconds dt)
{
    RendererLocator::get().Render(this);

    camera_.Update(dt);
}

void VkDrawSystem::Render()
{
}

void VkDrawSystem::Destroy()
{
    camera_.Destroy();
}

void VkDrawSystem::OnEvent(const SDL_Event& event)
{
    camera_.OnEvent(event);
}
}
