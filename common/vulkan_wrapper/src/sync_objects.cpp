#include "vk/sync_objects.h"

namespace neko::vk
{
SyncObjects::SyncObjects(const LogicalDevice& device, const Swapchain& swapchain)
	: device_(device), swapchain_(swapchain)
{
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight_.resize(swapchain_.GetImagesCount(), nullptr);
    imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
	
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
	{
		VkResult res = vkCreateFence(VkDevice(device_), &fenceInfo, nullptr, &inFlightFences_[i]);
		neko_assert(res == VK_SUCCESS, "Failed to create in flight fence for a frame!")
		res = vkCreateSemaphore(VkDevice(device_), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]);
		neko_assert(res == VK_SUCCESS, "Failed to create image semaphore for a frame!")
		res = vkCreateSemaphore(VkDevice(device_), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]);
		neko_assert(res == VK_SUCCESS, "Failed to create render semaphore for a frame!")
	}
}

SyncObjects::~SyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
        vkDestroyFence(VkDevice(device_), inFlightFences_[i], nullptr);
        vkDestroySemaphore(VkDevice(device_), renderFinishedSemaphores_[i], nullptr);
        vkDestroySemaphore(VkDevice(device_), imageAvailableSemaphores_[i], nullptr);
    }
}
}
