#include "vk/framebuffers/swapchain.h"

#include "vk/core/physical_device.h"

namespace neko::vk
{
void Swapchain::Init()
{
    const auto& vkObj = VkObjectsLocator::get();

    const auto& swapChainSupport =
            PhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice(vkObj.gpu), VkSurfaceKHR(vkObj.surface));

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
    createInfo.surface = VkSurfaceKHR(vkObj.surface);
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

    const QueueFamilyIndices& queueIndices = vkObj.gpu.GetQueueFamilyIndices();
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
    const VkResult res = vkCreateSwapchainKHR(VkDevice(vkObj.device), &createInfo, nullptr, &swapchain_);
    neko_assert(res == VK_SUCCESS, "Unable to create swap chain!")

    vkGetSwapchainImagesKHR(VkDevice(vkObj.device), swapchain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(VkDevice(vkObj.device), swapchain_, &imageCount, images_.data());

    format_ = surfaceFormat.format;
    extent_ = extent;

    CreateImageViews(vkObj.device);
}

void Swapchain::Init(Swapchain& oldSwapchain)
{
    const auto& vkObj = VkObjectsLocator::get();

    const auto& swapChainSupport =
            PhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice(vkObj.gpu), VkSurfaceKHR(vkObj.surface));

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
    createInfo.surface = VkSurfaceKHR(vkObj.surface);
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

    const QueueFamilyIndices& queueIndices = vkObj.gpu.GetQueueFamilyIndices();
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
    const VkResult res = vkCreateSwapchainKHR(VkDevice(vkObj.device), &createInfo, nullptr, &swapchain_);
    neko_assert(res == VK_SUCCESS, "Unable to create swap chain!")

    vkGetSwapchainImagesKHR(VkDevice(vkObj.device), swapchain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(VkDevice(vkObj.device), swapchain_, &imageCount, images_.data());

    format_ = surfaceFormat.format;
    extent_ = extent;

    CreateImageViews(vkObj.device);
}

void Swapchain::Destroy()
{
    const auto& vkObj = VkObjectsLocator::get();

    for (const auto& imageView : imageViews_)
        vkDestroyImageView(VkDevice(vkObj.device), imageView, nullptr);

    vkDestroySwapchainKHR(VkDevice(vkObj.device), swapchain_, nullptr);
}

VkResult Swapchain::AcquireNextImage(const VkSemaphore& presentCompleteSemaphore, VkFence fence)
{
    const auto& device = VkDevice(VkObjectsLocator::get().device);
    if (fence != VK_NULL_HANDLE)
        vkWaitForFences(device, 1, &fence, VK_TRUE, std::numeric_limits<std::uint64_t>::max());

    const VkResult res = vkAcquireNextImageKHR(device, swapchain_,
            std::numeric_limits<uint64_t>::max(), presentCompleteSemaphore,
            VK_NULL_HANDLE, &currentImage_);
    neko_assert(res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR,
            "Failed to acquire swapchain image")

    return res;
}

VkResult Swapchain::QueuePresent(VkQueue const& presentQueue, VkSemaphore const& waitSemaphore) const
{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &waitSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain_;
    presentInfo.pImageIndices = &currentImage_;
    return vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Swapchain::CreateImageViews(const LogicalDevice& device)
{
    imageViews_.resize(images_.size());
    for (size_t i = 0; i < images_.size(); i++)
    {
        imageViews_[i] = CreateImageView(VkDevice(device), images_[i], format_, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

bool Swapchain::CompareExtent(const VkExtent2D& extent2D) const
{
    return extent_.width == extent2D.width && extent_.height == extent2D.height;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == kFormat && availableFormat.colorSpace == kColorSpace)
			return availableFormat;

#ifdef VALIDATION_LAYERS
	std::cout << "Warning: no matching color format found, picking first available one\n";
#endif

	return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == kPresentationMode)
			return availablePresentMode;

#ifdef VALIDATION_LAYERS
	std::cout << "Unable to obtain preferred display mode, fallback to FIFO\n";
#endif

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
		return capabilities.currentExtent;

	auto& config = BasicEngine::GetInstance()->config;
	std::cout << config.windowSize.x << config.windowSize.y << '\n';

	VkExtent2D actualExtent = {config.windowSize.x, config.windowSize.y};

	actualExtent.width = std::max(capabilities.minImageExtent.width,
                               std::min(capabilities.maxImageExtent.width,
                                        actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height,
                               std::min(capabilities.maxImageExtent.height,
                                        actualExtent.height));

	return actualExtent;
}

VkImageView CreateImageView(
	const VkDevice& device, 
	const VkImage& image,
	const VkFormat format, 
	const VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	const VkResult res = vkCreateImageView(device, &viewInfo, nullptr,
	                                       &imageView);
	neko_assert(res == VK_SUCCESS, "Failed to create texture image view!")

	return imageView;
}
}
