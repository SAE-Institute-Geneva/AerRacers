#pragma once
#include "vk/vulkan_include.h"
#include "vk/vk_utilities.h"
#include "vk/core/logical_device.h"

namespace neko::vk
{
class Swapchain
{
public:
    explicit Swapchain() = default;

    void Init();
    void Init(const Swapchain& oldSwapchain);
    void Destroy();

    explicit operator const VkSwapchainKHR &() const { return swapchain_; }
    [[nodiscard]] const VkSwapchainKHR& GetSwapchain() const { return swapchain_; }

    [[nodiscard]] VkFormat GetFormat() const { return format_; }
    [[nodiscard]] const VkExtent2D& GetExtent() const { return extent_; }
	
	[[nodiscard]] const std::vector<VkImageView>& GetImageViews() const { return imageViews_; }
	
	[[nodiscard]] size_t GetImageCount() const { return images_.size(); }

private:
    void CreateImageViews(const LogicalDevice& device);

    VkSwapchainKHR swapchain_{};
    VkFormat format_ = kFormat;
    VkExtent2D extent_ = {};

    std::vector<VkImage> images_;
    std::vector<VkImageView> imageViews_;
    std::vector<VkFramebuffer> framebuffers_;
};

[[nodiscard]] static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
[[nodiscard]] static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
[[nodiscard]] static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

[[nodiscard]] static VkImageView CreateImageView(
	const VkDevice& device,
	const VkImage& image,
	VkFormat format,
	VkImageAspectFlags aspectFlags);
}