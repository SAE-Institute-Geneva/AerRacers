#include "vk/core/surface.h"

#include "SDL_vulkan.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
void Surface::Init(SDL_Window& window)
{
	neko_assert(SDL_Vulkan_CreateSurface(&window, VkResources::Inst->instance, &surface_),
		"Unable to create Vulkan compatible surface using SDL!")
}

void Surface::SetFormat()
{
	const VkResources* vkObj = VkResources::Inst;

	std::uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkObj->gpu, surface_, &formatCount, nullptr);

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkObj->gpu, surface_, &formatCount, surfaceFormats.data());

	if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		format_.format     = kFormat;
		format_.colorSpace = surfaceFormats[0].colorSpace;
	}
	else
	{
		bool foundFormat = false;
		for (auto&& surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == kFormat)
			{
				format_.format     = surfaceFormat.format;
				format_.colorSpace = surfaceFormat.colorSpace;
				foundFormat        = true;
				break;
			}
		}

		if (!foundFormat)
		{
			format_.format     = surfaceFormats[0].format;
			format_.colorSpace = surfaceFormats[0].colorSpace;
		}
	}
}

void Surface::Destroy() const
{
	vkDestroySurfaceKHR(VkResources::Inst->instance, surface_, nullptr);
}

VkSurfaceCapabilitiesKHR Surface::GetCapabilities() const
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkResources::Inst->gpu, surface_, &capabilities);
	return capabilities;
}
}    // namespace neko::vk
