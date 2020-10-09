#include "vk/vk_utilities.h"

namespace neko::vk
{
VkFormat FindSupportedFormat(
	const VkPhysicalDevice& gpu,
	const std::vector<VkFormat>& candidates,
	const VkImageTiling tiling,
	const VkFormatFeatureFlags features)
{
	for (const VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(gpu, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures &
			features) == features)
			return format;

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures &
			features) == features)
			return format;
	}

	neko_assert(false, "Failed to find supported format!")
}

VkFormat FindDepthFormat(const VkPhysicalDevice& gpu)
{
	return FindSupportedFormat(gpu,
	                           {
		                           VK_FORMAT_D32_SFLOAT,
		                           VK_FORMAT_D32_SFLOAT_S8_UINT,
		                           VK_FORMAT_D24_UNORM_S8_UINT
	                           },
	                           VK_IMAGE_TILING_OPTIMAL,
	                           VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}
}
