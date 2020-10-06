#include "vk/core/surface.h"

namespace neko::vk
{
Surface::Surface(const Instance& instance) : instance_(instance)
{}

void Surface::Init(const sdl::VulkanWindow& window)
{
    window.CreateSurface(VkInstance(instance_), surface_);
}

void Surface::Destroy()
{
    vkDestroySurfaceKHR(VkInstance(instance_), surface_, nullptr);
}
}