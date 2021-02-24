#include "vk/vk_resources.h"

namespace neko::vk
{
Image::Image(const VkFilter filter,
	const VkSamplerAddressMode addressMode,
	const VkSampleCountFlagBits samples,
	const VkImageLayout layout,
	const VkImageUsageFlags usage,
	const VkFormat format,
	const std::uint32_t mipLevels,
	const std::uint32_t arrayLayers,
	const VkExtent3D& extent)
   : extent_(extent),
	 format_(format),
	 sample_(samples),
	 usage_(usage),
	 mipLevels_(mipLevels),
	 arrayLayers_(arrayLayers),
	 filter_(filter),
	 addressMode_(addressMode),
	 layout_(layout)
{}

Image::Image(const ktxVulkanTexture& texture)
   : extent_({texture.width, texture.height, texture.depth}),
	 format_(texture.imageFormat),
	 sample_(),
	 usage_(),
	 mipLevels_(texture.levelCount),
	 arrayLayers_(texture.layerCount),
	 filter_(),
	 addressMode_(),
	 layout_(texture.imageLayout),
	 image_(texture.image),
	 memory_(texture.deviceMemory)
{}

void Image::Destroy() const
{
	const auto& device = VkResources::Inst->device;

	vkDestroyImageView(device, view_, nullptr);
	vkDestroySampler(device, sampler_, nullptr);
	vkDestroyImage(device, image_, nullptr);
	vkFreeMemory(device, memory_, nullptr);
}

VkDescriptorSetLayoutBinding Image::GetDescriptorSetLayout(
	uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stage)
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding                      = binding;
	descriptorSetLayoutBinding.descriptorType               = descriptorType;
	descriptorSetLayoutBinding.descriptorCount              = 1;
	descriptorSetLayoutBinding.stageFlags                   = stage;
	descriptorSetLayoutBinding.pImmutableSamplers           = nullptr;

	return descriptorSetLayoutBinding;
}

WriteDescriptorSet Image::GetWriteDescriptor(
	const std::uint32_t binding, const VkDescriptorType descriptorType) const
{
	VkDescriptorImageInfo imageInfo;
	imageInfo.sampler     = sampler_;
	imageInfo.imageView   = view_;
	imageInfo.imageLayout = layout_;

	VkWriteDescriptorSet descriptorWrite {};
	descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet          = VK_NULL_HANDLE;
	descriptorWrite.dstBinding      = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType  = descriptorType;

	return WriteDescriptorSet(descriptorWrite, imageInfo);
}

std::uint32_t Image::GetMipLevels(const VkExtent3D& extent)
{
	auto val = static_cast<float>(std::max(extent.width, std::max(extent.height, extent.depth)));
	return static_cast<std::uint32_t>(floorf(log2f(val)) + 1.0f);
}

bool Image::HasDepth(VkFormat format)
{
	return std::find(kDepthFormat.begin(), kDepthFormat.end(), format) != kDepthFormat.end();
}

bool Image::HasStencil(VkFormat format)
{
	return std::find(kStencilFormats.begin(), kStencilFormats.end(), format) !=
	       kStencilFormats.end();
}

VkImage Image::CreateImage(VkDeviceMemory& memory,
	const VkExtent3D extent,
	const VkFormat format,
	const VkSampleCountFlagBits samples,
	const VkImageTiling tiling,
	const VkImageUsageFlags usage,
	const VkMemoryPropertyFlags properties,
	const std::uint32_t mipLevels,
	const std::uint32_t arrayLayers,
	const VkImageType type)
{
	const auto& device = VkResources::Inst->device;
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.flags             = arrayLayers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCreateInfo.imageType         = type;
	imageCreateInfo.format            = format;
	imageCreateInfo.extent            = extent;
	imageCreateInfo.mipLevels         = mipLevels;
	imageCreateInfo.arrayLayers       = arrayLayers;
	imageCreateInfo.samples           = samples;
	imageCreateInfo.tiling            = tiling;
	imageCreateInfo.usage             = usage;
	imageCreateInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImage image;
	VkResult res = vkCreateImage(device, &imageCreateInfo, nullptr, &image);
	neko_assert(res == VK_SUCCESS, "Could not create image!")

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize       = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex =
		Buffer::FindMemoryType(memoryRequirements.memoryTypeBits, properties);
	res = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory);
	neko_assert(res == VK_SUCCESS, "Could not allocate memory for image!")

	res = vkBindImageMemory(device, image, memory, 0);
	neko_assert(res == VK_SUCCESS, "Could not bind image memory!")

	return image;
}

VkSampler Image::CreateImageSampler(
	VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, std::uint32_t mipLevels)
{
	const VkResources* vkObj = VkResources::Inst;
	VkPhysicalDeviceFeatures gpuFeatures;
	VkPhysicalDeviceProperties gpuProperties;
	vkGetPhysicalDeviceFeatures(vkObj->gpu, &gpuFeatures);
	vkGetPhysicalDeviceProperties(vkObj->gpu, &gpuProperties);

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter           = filter;
	samplerCreateInfo.minFilter           = filter;
	samplerCreateInfo.mipmapMode          = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU        = addressMode;
	samplerCreateInfo.addressModeV        = addressMode;
	samplerCreateInfo.addressModeW        = addressMode;
	samplerCreateInfo.mipLodBias          = 0.0f;
	samplerCreateInfo.anisotropyEnable    = static_cast<VkBool32>(anisotropic);
	samplerCreateInfo.maxAnisotropy =
		(anisotropic && gpuFeatures.samplerAnisotropy) ?
            std::min(kAnisotropy, gpuProperties.limits.maxSamplerAnisotropy) :
            1.0f;
	samplerCreateInfo.minLod                  = 0.0f;
	samplerCreateInfo.maxLod                  = static_cast<float>(mipLevels);
	samplerCreateInfo.borderColor             = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VkSampler sampler;
	const VkResult res = vkCreateSampler(vkObj->device, &samplerCreateInfo, nullptr, &sampler);
	neko_assert(res == VK_SUCCESS, "Could not create image sampler!")

	return sampler;
}

VkImageView Image::CreateImageView(const VkImage& image,
	const VkImageViewType type,
	const VkFormat format,
	const VkImageAspectFlags imageAspect,
	const std::uint32_t mipLevels,
	const std::uint32_t baseMipLevel,
	const std::uint32_t layerCount,
	const std::uint32_t baseArrayLayer)
{
	const auto& device = VkResources::Inst->device;

	VkImageView imageView;
	VkImageViewCreateInfo imageViewCreateInfo           = {};
	imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image                           = image;
	imageViewCreateInfo.viewType                        = type;
	imageViewCreateInfo.format                          = format;
	imageViewCreateInfo.components                      = {VK_COMPONENT_SWIZZLE_R,
        VK_COMPONENT_SWIZZLE_G,
        VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A};
	imageViewCreateInfo.subresourceRange.aspectMask     = imageAspect;
	imageViewCreateInfo.subresourceRange.baseMipLevel   = baseMipLevel;
	imageViewCreateInfo.subresourceRange.levelCount     = mipLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
	imageViewCreateInfo.subresourceRange.layerCount     = layerCount;

	const VkResult res = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView);
	neko_assert(res == VK_SUCCESS, "Could not create image view!")

	return imageView;
}

void Image::CreateMipmaps(const VkImage& image,
	VkExtent3D extent,
	VkFormat format,
	VkImageLayout dstImageLayout,
	std::uint32_t mipLevels,
	std::uint32_t baseArrayLayer,
	std::uint32_t layerCount)
{
	const auto& gpu = VkResources::Inst->gpu;

	// Get device properties for the requested texture format.
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(gpu, format, &formatProperties);

	// Mip-chain generation requires support for blit source and destination
	neko_assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT,
		"Hardware does not support blit")
	neko_assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT,
		"Hardware does not support blit")

	CommandBuffer commandBuffer(true);
	for (std::uint32_t i = 1; i < mipLevels; i++)
	{
		VkImageMemoryBarrier barrier0 {};
		barrier0.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier0.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier0.dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
		barrier0.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier0.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier0.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
		barrier0.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
		barrier0.image                           = image;
		barrier0.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier0.subresourceRange.baseMipLevel   = i - 1;
		barrier0.subresourceRange.levelCount     = 1;
		barrier0.subresourceRange.baseArrayLayer = baseArrayLayer;
		barrier0.subresourceRange.layerCount     = layerCount;
		vkCmdPipelineBarrier(VkCommandBuffer(commandBuffer),
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier0);

		VkImageBlit imageBlit {};
		imageBlit.srcOffsets[1] = {
			std::int32_t(extent.width >> (i - 1)), int32_t(extent.height >> (i - 1)), 1};
		imageBlit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.mipLevel       = i - 1;
		imageBlit.srcSubresource.baseArrayLayer = baseArrayLayer;
		imageBlit.srcSubresource.layerCount     = layerCount;
		imageBlit.dstOffsets[1] = {int32_t(extent.width >> i), int32_t(extent.height >> i), 1};
		imageBlit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.mipLevel       = i;
		imageBlit.dstSubresource.baseArrayLayer = baseArrayLayer;
		imageBlit.dstSubresource.layerCount     = layerCount;
		vkCmdBlitImage(VkCommandBuffer(commandBuffer),
			image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&imageBlit,
			VK_FILTER_LINEAR);

		VkImageMemoryBarrier barrier1 {};
		barrier1.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier1.srcAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
		barrier1.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
		barrier1.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier1.newLayout                       = dstImageLayout;
		barrier1.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
		barrier1.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
		barrier1.image                           = image;
		barrier1.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier1.subresourceRange.baseMipLevel   = i - 1;
		barrier1.subresourceRange.levelCount     = 1;
		barrier1.subresourceRange.baseArrayLayer = baseArrayLayer;
		barrier1.subresourceRange.layerCount     = layerCount;
		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier1);
	}

	VkImageMemoryBarrier barrier {};
	barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
	barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout                       = dstImageLayout;
	barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	barrier.image                           = image;
	barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel   = mipLevels - 1;
	barrier.subresourceRange.levelCount     = 1;
	barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	barrier.subresourceRange.layerCount     = layerCount;
	vkCmdPipelineBarrier(VkCommandBuffer(commandBuffer),
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	commandBuffer.SubmitIdle();
}

void Image::TransitionImageLayout(const VkImage& image,
	const VkImageLayout srcImageLayout,
	const VkImageLayout dstImageLayout,
	const VkImageAspectFlags imageAspect,
	const std::uint32_t mipLevels,
	const std::uint32_t baseMipLevel,
	const std::uint32_t layerCount,
	const std::uint32_t baseArrayLayer)
{
	CommandBuffer commandBuffer(true);
	VkImageMemoryBarrier imageMemoryBarrier            = {};
	imageMemoryBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout                       = srcImageLayout;
	imageMemoryBarrier.newLayout                       = dstImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image                           = image;
	imageMemoryBarrier.subresourceRange.aspectMask     = imageAspect;
	imageMemoryBarrier.subresourceRange.baseMipLevel   = baseMipLevel;
	imageMemoryBarrier.subresourceRange.levelCount     = mipLevels;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	imageMemoryBarrier.subresourceRange.layerCount     = layerCount;

	// Source access mask controls actions that have to be finished on the old layout before
	// it will be transitioned to the new layout.
	switch (srcImageLayout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED: imageMemoryBarrier.srcAccessMask = 0; break;
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default: break;
	}

	// Destination access mask controls the dependency for the new image layout.
	switch (dstImageLayout)
	{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.dstAccessMask =
				imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			if (imageMemoryBarrier.srcAccessMask == 0)
			{
				imageMemoryBarrier.srcAccessMask =
					VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}

			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default: break;
	}

	const VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	const VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		srcStageMask,
		dstStageMask,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&imageMemoryBarrier);

	commandBuffer.SubmitIdle();
}

VkFormat Image::FindSupportedFormat(const std::vector<VkFormat>& candidates,
	const VkImageTiling tiling,
	const VkFormatFeatureFlags features)
{
	const auto& gpu = VkResources::Inst->gpu;
	for (const VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(gpu, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			return format;

		if (tiling == VK_IMAGE_TILING_OPTIMAL &&
			(props.optimalTilingFeatures & features) == features)
			return format;
	}

	neko_assert(false, "Failed to find supported format!")
}

[[maybe_unused]] void Image::CopyBufferToImage(VkBuffer buffer,
	VkImage image,
	VkExtent3D extent,
	std::uint32_t layerCount,
	std::uint32_t baseArrayLayer)
{
	CommandBuffer commandBuffer(true);
	VkBufferImageCopy region               = {};
	region.bufferOffset                    = 0;
	region.bufferRowLength                 = 0;
	region.bufferImageHeight               = 0;
	region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = baseArrayLayer;
	region.imageSubresource.layerCount     = layerCount;
	region.imageOffset                     = {0, 0, 0};
	region.imageExtent                     = extent;
	vkCmdCopyBufferToImage(
		commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandBuffer.SubmitIdle();
}
}    // namespace neko::vk