#include "vk/core/surface.h"
#include "vk/graphics.h"

namespace neko::vk
{
void Surface::Init(SDL_Window& window)
{
    CreateSurface(window);
}

void Surface::SetFormat()
{
    const auto& vkObj = VkObjectsLocator::get();

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice(vkObj.gpu), surface_,
                                         &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice(vkObj.gpu), surface_,
                                         &formatCount, surfaceFormats.data());

    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        format_.format = VK_FORMAT_B8G8R8A8_UNORM;
        format_.colorSpace = surfaceFormats[0].colorSpace;
    }
    else
    {
        bool foundB8G8R8AUnorm = false;
        for (auto&& surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                format_.format = surfaceFormat.format;
                format_.colorSpace = surfaceFormat.colorSpace;
                foundB8G8R8AUnorm = true;
                break;
            }
        }

        if (!foundB8G8R8AUnorm)
        {
            format_.format = surfaceFormats[0].format;
            format_.colorSpace = surfaceFormats[0].colorSpace;
        }
    }
}

void Surface::Destroy() const
{
    const auto& vkObj = VkObjectsLocator::get();
    vkDestroySurfaceKHR(VkInstance(vkObj.instance), surface_, nullptr);
}

void Surface::CreateSurface(SDL_Window& window)
{
    const auto& instance = VkInstance(VkObjectsLocator::get().instance);
    neko_assert(SDL_Vulkan_CreateSurface(&window, instance, &surface_),
                "Unable to create Vulkan compatible surface using SDL!")
}
}