#include "vk/core/surface.h"
#include "vk/graphics.h"

namespace neko::vk
{
void Surface::Init()
{
    const auto& vkObj = VkResourcesLocator::get();

    vkObj.vkWindow->CreateSurface(VkInstance(vkObj.instance), surface_);
}

void Surface::Destroy() const
{
    const auto& vkObj = VkResourcesLocator::get();
    vkDestroySurfaceKHR(VkInstance(vkObj.instance), surface_, nullptr);
}
}