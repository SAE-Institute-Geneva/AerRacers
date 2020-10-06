#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
Swapchain::Swapchain(const LogicalDevice& device) : device_(device)
{}

void Swapchain::Init(const PhysicalDevice& gpu, const Surface& surface)
{
    const auto& swapChainSupport = QuerySwapChainSupport(VkPhysicalDevice(gpu), VkSurfaceKHR(surface));

    const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    // Populate swap chain creation info
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = VkSurfaceKHR(surface);
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;

    const QueueFamilyIndices& queueIndices = gpu.GetQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = {
            queueIndices.graphicsFamily,
            queueIndices.presentFamily
    };
    if (queueIndices.graphicsFamily != queueIndices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // Create new one
    const VkResult res = vkCreateSwapchainKHR(VkDevice(device_), &createInfo, nullptr, &swapchain_);
    neko_assert(res == VK_SUCCESS, "Unable to create swap chain!")

    vkGetSwapchainImagesKHR(VkDevice(device_), swapchain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(VkDevice(device_), swapchain_, &imageCount, images_.data());

    format_ = surfaceFormat.format;
    extent_ = extent;

    CreateImageViews();
}

void Swapchain::Init(const PhysicalDevice& gpu, const Surface& surface, const Swapchain& oldSwapchain)
{
    const auto& swapChainSupport = QuerySwapChainSupport(VkPhysicalDevice(gpu), VkSurfaceKHR(surface));

    const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);
    extent_ = extent;
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    // Populate swap chain creation info
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = VkSurfaceKHR(surface);
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VkSwapchainKHR(oldSwapchain);

    const QueueFamilyIndices& queueIndices = gpu.GetQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = {
            queueIndices.graphicsFamily,
            queueIndices.presentFamily
    };
    if (queueIndices.graphicsFamily != queueIndices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // Create new one
    const VkResult res = vkCreateSwapchainKHR(VkDevice(device_), &createInfo, nullptr, &swapchain_);
    neko_assert(res == VK_SUCCESS, "Unable to create swap chain!")

    vkGetSwapchainImagesKHR(VkDevice(device_), swapchain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(VkDevice(device_), swapchain_, &imageCount, images_.data());

    format_ = surfaceFormat.format;
    extent_ = extent;

    CreateImageViews();
}

void Swapchain::Destroy()
{
    vkDestroySwapchainKHR(VkDevice(device_), swapchain_, nullptr);

    for (const auto& imageView : imageViews_)
        vkDestroyImageView(VkDevice(device_), imageView, nullptr);
}

void Swapchain::CreateImageViews()
{
    imageViews_.resize(images_.size());
    for (size_t i = 0; i < images_.size(); i++)
    {
        imageViews_[i] = CreateImageView(VkDevice(device_), images_[i], format_, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}
}