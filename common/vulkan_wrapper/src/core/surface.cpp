#include "vk/core/surface.h"

namespace neko::vk
{
Surface::Surface(const Instance& instance, SDL_Window* window) : instance_(instance)
{
    neko_assert(SDL_Vulkan_CreateSurface(window, VkInstance(instance_), &surface_),
            "Unable to create Vulkan compatible surface using SDL!")

    SurfaceLocator::provide(this);
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(VkInstance(instance_), surface_, nullptr);
}
}