#pragma once
#include "vk/vulkan_include.h"
#include "vk/vk_utilities.h"

namespace neko::vk
{
class Swapchain
{
public:
    explicit Swapchain(const LogicalDevice& device);

    void Init(const PhysicalDevice& gpu, const Surface& surface);
    void Init(const PhysicalDevice& gpu, const Surface& surface, const Swapchain& oldSwapchain);
    void Destroy();

    explicit operator const VkSwapchainKHR &() const { return swapchain_; }
    [[nodiscard]] const VkSwapchainKHR& GetSwapchain() const { return swapchain_; }

    [[nodiscard]] VkFormat GetFormat() const { return format_; }
    [[nodiscard]] const VkExtent2D& GetExtent() const { return extent_; }
	
	[[nodiscard]] const std::vector<VkImageView>& GetImageViews() const { return imageViews_; }
	
	[[nodiscard]] size_t GetImagesCount() const { return images_.size(); }

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