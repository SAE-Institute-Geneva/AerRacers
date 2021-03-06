#include "vk/vk_resources.h"

namespace neko::vk
{
void LogicalDevice::Init()
{
	const VkResources* vkObj                     = VkResources::Inst;
	const QueueFamilyIndices& queueFamilyIndices = vkObj->gpu.GetQueueFamilyIndices();

	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	if (queueFamilyIndices.graphicsFamily == queueFamilyIndices.presentFamily)
	{
		VkDeviceQueueCreateInfo queueCreateInfo {};
		queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	else
	{
		std::uint32_t uniqueQueueFamilies[] = {
			queueFamilyIndices.graphicsFamily,
			queueFamilyIndices.presentFamily,
		};
		for (std::uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo {};
			queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount       = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
	}

	VkPhysicalDeviceFeatures deviceFeatures {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	// Device creation information
	VkDeviceCreateInfo createInfo {};
	createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos    = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<std::uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures     = &deviceFeatures;

#ifdef VALIDATION_LAYERS
	createInfo.enabledLayerCount   = static_cast<std::uint32_t>(kValidationLayers.size());
	createInfo.ppEnabledLayerNames = kValidationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
#endif

	createInfo.enabledExtensionCount   = static_cast<std::uint32_t>(kDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = kDeviceExtensions.data();

	// Finally we're ready to create a new device
	const VkResult res = vkCreateDevice(vkObj->gpu, &createInfo, nullptr, &device_);
	vkCheckError(res, "Failed to create logical device!");

	vkGetDeviceQueue(device_, queueFamilyIndices.graphicsFamily, 0, &graphicsQueue_);
	vkGetDeviceQueue(device_, queueFamilyIndices.presentFamily, 0, &presentQueue_);
}

void LogicalDevice::Destroy() const
{
	vkDeviceWaitIdle(device_);
	vkDestroyDevice(device_, nullptr);
}
}    // namespace neko::vk