#include "vk/vk_resources.h"

namespace neko::vk
{
void Swapchain::Init()
{
	const VkResources* vkObj               = VkResources::Inst;
	const auto swapChainSupport            = vkObj->gpu.QuerySwapChainSupport(vkObj->surface);
	const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	const VkPresentModeKHR presentMode     = ChooseSwapPresentMode(swapChainSupport.presentModes);
	std::uint32_t imageCount               = swapChainSupport.capabilities.minImageCount + 1;
	ChooseSwapExtent(swapChainSupport.capabilities);

	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	// Populate swap chain creation info
	VkSwapchainCreateInfoKHR createInfo {};
	createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface          = vkObj->surface;
	createInfo.minImageCount    = imageCount;
	createInfo.imageFormat      = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent      = extent_;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform     = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode      = presentMode;
	createInfo.clipped          = VK_TRUE;
	createInfo.oldSwapchain     = {};

	const QueueFamilyIndices& queueIndices = vkObj->gpu.GetQueueFamilyIndices();
	std::uint32_t queueFamilyIndices[] = {queueIndices.graphicsFamily, queueIndices.presentFamily};
	if (queueIndices.graphicsFamily != queueIndices.presentFamily)
	{
		createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices   = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices   = nullptr;
	}

	// Create new one
	vkCheckError(vkCreateSwapchainKHR(vkObj->device, &createInfo, nullptr, &swapchain_),
		"Unable to create swap chain!");

	vkGetSwapchainImagesKHR(vkObj->device, swapchain_, &imageCount, nullptr);
	images_.resize(imageCount);
	vkGetSwapchainImagesKHR(vkObj->device, swapchain_, &imageCount, images_.data());

	CreateImageViews(vkObj->device);
}

void Swapchain::Init(Swapchain& oldSwapchain)
{
	const VkResources* vkObj    = VkResources::Inst;
	const auto swapChainSupport = vkObj->gpu.QuerySwapChainSupport(vkObj->surface);

	const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	const VkPresentModeKHR presentMode     = ChooseSwapPresentMode(swapChainSupport.presentModes);

	ChooseSwapExtent(swapChainSupport.capabilities);
	std::uint32_t imageCount               = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	// Populate swap chain creation info
	VkSwapchainCreateInfoKHR createInfo {};
	createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface          = vkObj->surface;
	createInfo.minImageCount    = imageCount;
	createInfo.imageFormat      = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent      = extent_;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform     = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode      = presentMode;
	createInfo.clipped          = VK_TRUE;
	createInfo.oldSwapchain     = VkSwapchainKHR(oldSwapchain);

	const QueueFamilyIndices& queueIndices = vkObj->gpu.GetQueueFamilyIndices();
	std::uint32_t queueFamilyIndices[] = {queueIndices.graphicsFamily, queueIndices.presentFamily};
	if (queueIndices.graphicsFamily != queueIndices.presentFamily)
	{
		createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices   = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices   = nullptr;
	}

	// Create new one
	vkCheckError(vkCreateSwapchainKHR(vkObj->device, &createInfo, nullptr, &swapchain_),
		"Unable to create swap chain!");

	vkGetSwapchainImagesKHR(vkObj->device, swapchain_, &imageCount, nullptr);
	images_.resize(imageCount);
	vkGetSwapchainImagesKHR(vkObj->device, swapchain_, &imageCount, images_.data());

	CreateImageViews(vkObj->device);
}

void Swapchain::Destroy()
{
	const LogicalDevice& device = VkResources::Inst->device;
	for (const auto& imageView : imageViews_) vkDestroyImageView(device, imageView, nullptr);

	vkDestroySwapchainKHR(device, swapchain_, nullptr);
}

VkResult Swapchain::AcquireNextImage(VkSemaphore presentCompleteSemaphore, VkFence fence)
{
	const LogicalDevice& device = VkResources::Inst->device;
	if (fence) vkWaitForFences(device, 1, &fence, VK_TRUE, kDefaultFenceTimeout);

	const VkResult res = vkAcquireNextImageKHR(device,
		swapchain_,
		kDefaultFenceTimeout,
		presentCompleteSemaphore,
		nullptr,
		&currentImage_);
	vkCheckError(res, "Failed to acquire swapchain image!");

	return res;
}

VkResult Swapchain::QueuePresent(VkQueue presentQueue, VkSemaphore waitSemaphore) const
{
	VkPresentInfoKHR presentInfo   = {};
	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = &waitSemaphore;
	presentInfo.swapchainCount     = 1;
	presentInfo.pSwapchains        = &swapchain_;
	presentInfo.pImageIndices      = &currentImage_;
	return vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Swapchain::CreateImageViews(VkDevice device)
{
	imageViews_.resize(images_.size());
	for (size_t i = 0; i < images_.size(); i++)
	{
		VkImageViewCreateInfo viewInfo {};
		viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image                           = images_[i];
		viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format                          = format_;
		viewInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel   = 0;
		viewInfo.subresourceRange.levelCount     = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount     = 1;

		vkCheckError(vkCreateImageView(device, &viewInfo, nullptr, &imageViews_[i]),
			"Failed to create texture image view!");
	}
}

bool Swapchain::CompareExtent(const VkExtent2D& extent2D) const
{
	return extent_.width == extent2D.width && extent_.height == extent2D.height;
}

VkSurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == kFormat && availableFormat.colorSpace == kColorSpace)
		{
			format_ = availableFormat.format;
			return availableFormat;
		}
	}

#ifdef VALIDATION_LAYERS
	std::cout << "Warning: no matching color format found, picking first available one\n";
#endif

	format_ = availableFormats[0].format;
	return availableFormats[0];
}

VkPresentModeKHR Swapchain::ChooseSwapPresentMode(
	const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	const bool vsync = BasicEngine::GetInstance()->GetConfig().flags & Configuration::VSYNC;
	VkPresentModeKHR presentMode = vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == presentMode) return availablePresentMode;

#ifdef VALIDATION_LAYERS
	std::cout << "Unable to obtain preferred display mode, fallback to FIFO\n";
#endif

	return VK_PRESENT_MODE_FIFO_KHR;
}

void Swapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		extent_ = capabilities.currentExtent;
		return;
	}

	auto& config   = BasicEngine::GetInstance()->GetConfig();
	extent_.width  = std::max(capabilities.minImageExtent.width,
        std::min(capabilities.minImageExtent.width, config.windowSize.x));
	extent_.height = std::max(capabilities.minImageExtent.height,
		std::min(capabilities.minImageExtent.height, config.windowSize.y));
}
}    // namespace neko::vk
