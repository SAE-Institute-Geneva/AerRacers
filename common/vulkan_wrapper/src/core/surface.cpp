#include "vk/core/surface.h"

namespace neko::vk
{
void Surface::Init(const sdl::VulkanWindow* window, const Instance& instance)
{
    window->CreateSurface(VkInstance(instance), surface_);
}

void Surface::Destroy(const Instance& instance) const
{
    vkDestroySurfaceKHR(VkInstance(instance), surface_, nullptr);
}
}