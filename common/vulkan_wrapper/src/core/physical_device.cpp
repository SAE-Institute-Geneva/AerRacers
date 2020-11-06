#include "vk/core/physical_device.h"

#include <cstring>
#include <sstream>

#include "vk/graphics.h"

namespace neko::vk
{
static const std::array<VkSampleCountFlagBits, 6> kStageFlagBits =
{
        VK_SAMPLE_COUNT_64_BIT,
        VK_SAMPLE_COUNT_32_BIT,
        VK_SAMPLE_COUNT_16_BIT,
        VK_SAMPLE_COUNT_8_BIT,
        VK_SAMPLE_COUNT_4_BIT,
        VK_SAMPLE_COUNT_2_BIT
};

void PhysicalDevice::Init()
{
    const auto& vkObj = VkObjectsLocator::get();

    std::uint32_t physDeviceCount(0);
    vkEnumeratePhysicalDevices(VkInstance(vkObj.instance), &physDeviceCount, nullptr);
    neko_assert(physDeviceCount != 0, "No GPU found!")
#ifdef VALIDATION_LAYERS
    std::ostringstream oss1;
    oss1 << "Found " << physDeviceCount << " GPUs:\n";
    logDebug(oss1.str());
#endif

    std::vector<VkPhysicalDevice> physDevices(physDeviceCount);
    const VkResult res = vkEnumeratePhysicalDevices(VkInstance(vkObj.instance), &physDeviceCount, physDevices.data());
    neko_assert(res == VK_SUCCESS, "Enumerating GPUs failed!")
    neko_assert(physDeviceCount != 0, "No GPUs that supports Vulkan found!")

    VkPhysicalDeviceProperties deviceProperties;
    for (auto& gpu : physDevices)
    {
        vkGetPhysicalDeviceProperties(gpu, &deviceProperties);
#ifdef VALIDATION_LAYERS
        std::ostringstream oss2;
        oss2 << "Testing if '" << deviceProperties.deviceName << "' is a suitable GPU\n";
        logDebug(oss2.str());
#endif

        queueFamilyIndices_ = FindQueueFamilies(gpu, VkSurfaceKHR(vkObj.surface));
        if (IsDeviceSuitable(gpu, VkSurfaceKHR(vkObj.surface), queueFamilyIndices_))
        {
            gpu_ = gpu;
#ifdef VALIDATION_LAYERS
            std::ostringstream oss3;
            oss3 << "'" << deviceProperties.deviceName << "' is a suitable GPU\n";
            logDebug(oss3.str());
#endif
            break;
        }
    }

    neko_assert(gpu_, "No suitable GPU found!")
#ifdef VALIDATION_LAYERS
    std::ostringstream oss4;
    oss4 << "Picked '" << deviceProperties.deviceName << "' as the physical device";
    logDebug(oss4.str());
#endif

    // Find the number queues this device supports
    std::uint32_t familyQueueCount(0);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu_, &familyQueueCount, nullptr);
    neko_assert(familyQueueCount != 0, "Device has no family of queues associated with it")

    // Extract the properties of all the queue families
    std::vector<VkQueueFamilyProperties> queueProperties(familyQueueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu_, &familyQueueCount, queueProperties.data());

    // Make sure the family of commands contains an option to issue graphical commands.
    std::uint32_t queueNodeIndex = INVALID_INDEX;
    for (std::uint32_t i = 0; i < familyQueueCount; i++)
    {
        if (queueProperties[i].queueCount > 0 && queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueNodeIndex = i;
            break;
        }
    }

    neko_assert(queueNodeIndex != INVALID_INDEX,
                "Unable to find a queue command family that accepts graphics commands")
}

VkSampleCountFlagBits PhysicalDevice::GetMsaaSamples() const
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(gpu_, &physicalDeviceProperties);

    const auto kCounts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts,
                                  physicalDeviceProperties.limits.framebufferDepthSampleCounts);

    for (const auto& kSampleFlag : kStageFlagBits)
        if (kCounts & kSampleFlag) return kSampleFlag;

    return VK_SAMPLE_COUNT_1_BIT;
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilies(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface)
{
	QueueFamilyIndices indices;

	std::uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount,
	                                         queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		if (presentSupport)
			indices.presentFamily = i;

		if (indices.IsComplete())
			break;

		i++;
	}

#ifdef VALIDATION_LAYERS
	if (indices.graphicsFamily != INVALID_INDEX)
	{
		std::cout << "Queue family #" << indices.graphicsFamily <<
			" supports graphics\n";

		if (indices.presentFamily != INVALID_INDEX)
			std::cout << "Queue family #" << indices.presentFamily <<
				" supports presentation\n";
		else
			std::cout <<
				"could not find a valid queue family with present support!\n";
	}
	else
		std::cout <<
			"could not find a valid queue family with graphics support!\n";
	std::cout << '\n';
#endif

	return indices;
}

bool PhysicalDevice::CheckDeviceExtensionSupport(const VkPhysicalDevice& gpu)
{
	std::uint32_t extensionCount = 0;
	VkResult res = vkEnumerateDeviceExtensionProperties(
		gpu, nullptr, &extensionCount, nullptr);
	neko_assert(res == VK_SUCCESS,
	            "Unable to query vulkan device extensions count")

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	res = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount,
	                                           availableExtensions.data());
	neko_assert(res == VK_SUCCESS,
	            "Unable to retrieve vulkan device extension names")

#ifdef VALIDATION_LAYERS
	// Display layer names and find the ones we specified above
	std::cout << "Found " << extensionCount << " device extensions:\n";
	std::uint32_t count(0);
	for (const auto& availableExtension : availableExtensions)
	{
		std::cout << count << ": " << availableExtension.extensionName << '\n';
		count++;
	}
#endif

	for (const auto& extName : kDeviceExtensions)
	{
		bool layerFound = false;
		for (const auto& extProperties : availableExtensions)
		{
			if (strcmp(extName, extProperties.extensionName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound) return false;
	}

#ifdef VALIDATION_LAYERS
	// Print the ones we're enabling
	for (const auto& extension : kDeviceExtensions)
		std::cout << "Applying device extension: " << extension << "\n";
	std::cout << "\n";
#endif

	return true;
}

bool PhysicalDevice::IsDeviceSuitable(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface,
                      const QueueFamilyIndices& queueFamilyIndices)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(gpu, &deviceProperties);
	vkGetPhysicalDeviceFeatures(gpu, &deviceFeatures);

	const bool extensionsSupported = CheckDeviceExtensionSupport(gpu);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(gpu, VkSurfaceKHR(surface));
		swapChainAdequate = !swapChainSupport.formats.empty() && !
			swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(gpu, &supportedFeatures);

	return queueFamilyIndices.IsComplete() && extensionsSupported &&
		swapChainAdequate &&
		supportedFeatures.samplerAnisotropy &&
		deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader;
}

SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface)
{
	//Fill swap chain details
	SwapChainSupportDetails details{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface,
	                                          &details.capabilities);

	//Fill swap chain formats data
	std::uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount,
		                                     details.formats.data());
	}

	//Fill swap chain presentation modes data
	std::uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount,
	                                          nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			gpu, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}
}