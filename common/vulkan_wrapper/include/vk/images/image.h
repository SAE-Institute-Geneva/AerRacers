#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include <array>

#include "vulkan/vulkan.h"

#include "ktxvulkan.h"

#include "graphics/texture.h"

#include "vk/descriptors/descriptor_interface.h"

namespace neko::vk
{
constexpr float kAnisotropy = 16.0f;
constexpr std::array<VkFormat, 6> kDepthFormat = {
	VK_FORMAT_D16_UNORM,
	VK_FORMAT_X8_D24_UNORM_PACK32,
	VK_FORMAT_D32_SFLOAT,
	VK_FORMAT_D16_UNORM_S8_UINT,
	VK_FORMAT_D24_UNORM_S8_UINT,
	VK_FORMAT_D32_SFLOAT_S8_UINT,
};

constexpr std::array<VkFormat, 4> kStencilFormats = {
	VK_FORMAT_S8_UINT,
	VK_FORMAT_D16_UNORM_S8_UINT,
	VK_FORMAT_D24_UNORM_S8_UINT,
	VK_FORMAT_D32_SFLOAT_S8_UINT,
};

class Image : public IDescriptor
{
public:
	Image(VkFilter filter,
		VkSamplerAddressMode addressMode,
		VkSampleCountFlagBits samples,
		VkImageLayout layout,
		VkImageUsageFlags usage,
		VkFormat format,
		std::uint32_t mipLevels,
		std::uint32_t arrayLayers,
		const VkExtent3D& extent);
	explicit Image(const ktxVulkanTexture& texture);

	void Destroy() const override;

	static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(
		std::uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stage);

	[[nodiscard]] WriteDescriptorSet GetWriteDescriptor(
		std::uint32_t binding, VkDescriptorType descriptorType) const override;

	[[nodiscard]] const VkFormat& GetFormat() const { return format_; }
	[[nodiscard]] const VkImage& GetImage() const { return image_; }
	[[nodiscard]] const VkDeviceMemory& GetMemory() const { return memory_; }
	[[nodiscard]] const VkSampler& GetSampler() { return sampler_; }
	[[nodiscard]] const VkImageView& GetView() const { return view_; }

	static std::uint32_t GetMipLevels(const VkExtent3D& extent);

	static bool HasDepth(VkFormat format);
	static bool HasStencil(VkFormat format);

protected:
	static VkImage CreateImage(VkDeviceMemory& memory,
		VkExtent3D extent,
		VkFormat format,
		VkSampleCountFlagBits samples,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		std::uint32_t mipLevels,
		std::uint32_t arrayLayers,
		VkImageType type);

	static VkSampler CreateImageSampler(VkFilter filter,
		VkSamplerAddressMode addressMode,
		bool anisotropic,
		std::uint32_t mipLevels);

	static VkImageView CreateImageView(const VkImage& image,
		VkImageViewType type,
		VkFormat format,
		VkImageAspectFlags imageAspect,
		std::uint32_t mipLevels,
		std::uint32_t baseMipLevel,
		std::uint32_t layerCount,
		std::uint32_t baseArrayLayer);

	static void CreateMipmaps(const VkImage& image,
		VkExtent3D extent,
		VkFormat format,
		VkImageLayout dstImageLayout,
		std::uint32_t mipLevels,
		std::uint32_t baseArrayLayer,
		std::uint32_t layerCount);

	static void TransitionImageLayout(const VkImage& image,
		VkImageLayout srcImageLayout,
		VkImageLayout dstImageLayout,
		VkImageAspectFlags imageAspect,
		std::uint32_t mipLevels,
		std::uint32_t baseMipLevel,
		std::uint32_t layerCount,
		std::uint32_t baseArrayLayer);

	static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features);

	[[maybe_unused]] static void CopyBufferToImage(VkBuffer buffer,
		VkImage image,
		VkExtent3D extent,
		uint32_t layerCount,
		uint32_t baseArrayLayer);

	VkExtent3D extent_;
	VkFormat format_;
	VkSampleCountFlagBits sample_;
	VkImageUsageFlags usage_ {};
	std::uint32_t mipLevels_;
	std::uint32_t arrayLayers_;

	VkFilter filter_;
	VkSamplerAddressMode addressMode_;

	VkImageLayout layout_;

	VkImage image_ {};
	VkDeviceMemory memory_ {};
	VkSampler sampler_ {};
	VkImageView view_ {};
};
}    // namespace neko::vk
