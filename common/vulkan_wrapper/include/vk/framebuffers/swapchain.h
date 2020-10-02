#pragma once
#include "vk/vulkan_include.h"
#include "vk/vk_utilities.h"

namespace neko::vk
{
class Swapchain
{
public:
    explicit Swapchain(const LogicalDevice& device);
    Swapchain(const LogicalDevice& device, const Swapchain& oldSwapchain);
    ~Swapchain();

    explicit operator const VkSwapchainKHR &() const { return swapchain_; }
    [[nodiscard]] const VkSwapchainKHR& GetSwapchain() const { return swapchain_; }

    [[nodiscard]] VkFormat GetFormat() const { return format_; }
    const VkExtent2D& GetExtent() const { return extent_; }
	
	const std::vector<VkImageView>& GetImageViews() const { return imageViews_; }
	
	size_t GetImagesCount() const { return images_.size(); }

private:
    void CreateImageViews();

    const LogicalDevice& device_;

    VkSwapchainKHR swapchain_{};
    VkFormat format_ = kFormat;
    VkExtent2D extent_ = {};

    std::vector<VkImage> images_;
    std::vector<VkImageView> imageViews_;
    std::vector<VkFramebuffer> framebuffers_;
};
}