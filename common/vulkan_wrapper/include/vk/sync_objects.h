#pragma once
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
const int MAX_FRAMES_IN_FLIGHT = 2;

class SyncObjects
{
public:
	SyncObjects(const LogicalDevice& device, const Swapchain& swapchain);
	~SyncObjects();

	const std::vector<VkFence>& GetInFlightFences() const { return inFlightFences_; }
	std::vector<VkFence>& GetImagesInFlight() { return imagesInFlight_; }
	const std::vector<VkSemaphore>& GetImageAvailableSemaphores() const { return imageAvailableSemaphores_; }
	const std::vector<VkSemaphore>& GetRenderFinishedSemaphores() const { return renderFinishedSemaphores_; }
	
private:
	const LogicalDevice& device_;
	const Swapchain& swapchain_;
	
	std::vector<VkFence> inFlightFences_;
	std::vector<VkFence> imagesInFlight_;
	std::vector<VkSemaphore> imageAvailableSemaphores_;
	std::vector<VkSemaphore> renderFinishedSemaphores_;
};
}
