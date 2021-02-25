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
#include "vk/images/image.h"

namespace neko::vk
{
constexpr VkImageTiling kTiling = VK_IMAGE_TILING_OPTIMAL;
constexpr VkImageUsageFlags kUsage =
	VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
constexpr VkImageAspectFlagBits kAspect = VK_IMAGE_ASPECT_COLOR_BIT;
constexpr VkImageLayout kLayout         = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

class Image2d final : public Image
{
public:
	Image2d(std::string_view filename,
		VkFormat format                  = VK_FORMAT_R8G8B8A8_SRGB,
		VkFilter filter                  = VK_FILTER_LINEAR,
		VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		bool anisotropic                 = true,
		bool mipmap                      = true,
		bool load                        = true);

	Image2d(Vec2u extent,
		VkFormat format                  = VK_FORMAT_R8G8B8A8_SRGB,
		VkImageLayout layout             = kLayout,
		VkImageUsageFlags usage          = kUsage,
		VkFilter filter                  = VK_FILTER_LINEAR,
		VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkSampleCountFlagBits samples    = VK_SAMPLE_COUNT_1_BIT,
		bool anisotropic                 = false,
		bool mipmap                      = false);

	void Load();
	void LoadKtx();

	void CreateFromKtx(const ktxVulkanTexture& texture);

	[[nodiscard]] static constexpr VkImageType GetType() { return VK_IMAGE_TYPE_2D; }
	[[nodiscard]] static constexpr VkImageViewType GetViewType() { return VK_IMAGE_VIEW_TYPE_2D; }

	void SetFromKtxVkTexture(const ktxVulkanTexture& texture);
	void SetFilePath(std::string_view filePath) { filePath_ = filePath; }
	[[nodiscard]] std::string_view GetFilePath() const { return filePath_; }

private:
	std::string filePath_ = {};

	bool anisotropic_;
	bool mipmap_;

	std::uint32_t components_ = 4;
};
}    // namespace neko::vk