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
#include "vk/core/surface.h"

namespace neko::vk
{
constexpr VkColorSpaceKHR kColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

class Swapchain
{
public:
	explicit Swapchain() = default;

	operator const VkSwapchainKHR&() const { return swapchain_; }

	void Init();
	void Init(Swapchain& oldSwapchain);
	void Destroy();

	VkResult AcquireNextImage(VkSemaphore presentCompleteSemaphore, VkFence fence);
	[[nodiscard]] VkResult QueuePresent(VkQueue presentQueue, VkSemaphore waitSemaphore) const;

	[[nodiscard]] VkFormat GetFormat() const { return format_; }
	[[nodiscard]] const VkExtent2D& GetExtent() const { return extent_; }
	[[nodiscard]] const std::vector<VkImageView>& GetImageViews() const { return imageViews_; }
	[[nodiscard]] std::uint32_t GetCurrentImageIndex() const { return currentImage_; }
	[[nodiscard]] std::uint32_t GetImageCount() const
	{
		return static_cast<std::uint32_t>(images_.size());
	}

	[[nodiscard]] bool CompareExtent(const VkExtent2D& extent2D) const;

private:
	void CreateImageViews(VkDevice device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& availableFormats);
	static VkPresentModeKHR ChooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& availablePresentModes);
	void ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkSwapchainKHR swapchain_ {};
	VkFormat format_   = kFormat;
	VkExtent2D extent_ = {};

	std::vector<VkImage> images_;
	std::vector<VkImageView> imageViews_;
	std::vector<VkFramebuffer> framebuffers_;
	std::uint32_t currentImage_ = std::numeric_limits<uint32_t>::max();
};
}    // namespace neko::vk