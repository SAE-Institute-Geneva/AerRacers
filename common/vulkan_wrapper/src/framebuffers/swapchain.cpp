#include "vk/framebuffers/swapchain.h"

#include "vk/core/logical_device.h"

namespace neko::vk
{
void Swapchain::Init(const PhysicalDevice& gpu, const LogicalDevice& device, const Surface& surface)
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
    const VkResult res = vkCreateSwapchainKHR(VkDevice(device), &createInfo, nullptr, &swapchain_);
    neko_assert(res == VK_SUCCESS, "Unable to create swap chain!")

    vkGetSwapchainImagesKHR(VkDevice(device), swapchain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(VkDevice(device), swapchain_, &imageCount, images_.data());

    format_ = surfaceFormat.format;
    extent_ = extent;

    CreateImageViews(device);
}

void Swapchain::Init(const PhysicalDevice& gpu, const LogicalDevice& device, const Surface& surface, const Swapchain& oldSwapchain)
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
    const VkResult res = vkCreateSwapchainKHR(VkDevice(device), &createInfo, nullptr, &swapchain_);
    neko_assert(res == VK_SUCCESS, "Unable to create swap chain!")

    vkGetSwapchainImagesKHR(VkDevice(device), swapchain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(VkDevice(device), swapchain_, &imageCount, images_.data());

    format_ = surfaceFormat.format;
    extent_ = extent;

    CreateImageViews(device);
}

void Swapchain::Destroy(const LogicalDevice& device)
{
    for (const auto& imageView : imageViews_)
        vkDestroyImageView(VkDevice(device), imageView, nullptr);
	
    vkDestroySwapchainKHR(VkDevice(device), swapchain_, nullptr);
}

void Swapchain::CreateImageViews(const LogicalDevice& device)
{
    imageViews_.resize(images_.size());
    for (size_t i = 0; i < images_.size(); i++)
    {
        imageViews_[i] = CreateImageView(VkDevice(device), images_[i], format_, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == kFormat && availableFormat.colorSpace ==
			kColorSpace)
			return availableFormat;

#ifdef VALIDATION_LAYERS
	std::cout <<
		"Warning: no matching color format found, picking first available one\n";
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
