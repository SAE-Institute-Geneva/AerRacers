#include "vk/images/image_depth.h"

namespace neko::vk
{
ImageDepth::ImageDepth(const Vec2u& extent, VkSampleCountFlagBits samples)
   : Image(VK_FILTER_LINEAR,
		 VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		 samples,
		 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		 FindSupportedFormat(kAvailableFormats,
			 VK_IMAGE_TILING_OPTIMAL,
			 VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
		 1,
		 1,
		 {static_cast<std::uint32_t>(extent.x), static_cast<std::uint32_t>(extent.y), 1})
{
	neko_assert(format_ != VK_FORMAT_UNDEFINED, "Invalid depth format!");

	VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (HasStencil(format_)) aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	image_ = CreateImage(extent_,
		format_,
		sample_,
		VK_IMAGE_TILING_OPTIMAL,
		usage_,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		1,
		1,
		VK_IMAGE_TYPE_2D);

	sampler_ = CreateImageSampler(filter_, addressMode_, false, 1);

	view_ = CreateImageView(
		image_, VK_IMAGE_VIEW_TYPE_2D, format_, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 0, 1, 0);
	TransitionImageLayout(image_, VK_IMAGE_LAYOUT_UNDEFINED, layout_, aspectMask, 1, 0, 1, 0);
}
}    // namespace neko::vk