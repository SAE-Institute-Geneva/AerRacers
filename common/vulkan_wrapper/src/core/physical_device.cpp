#include "vk/core/physical_device.h"
#include "vk/vk_utilities.h"

namespace neko::vk
{
PhysicalDevice::PhysicalDevice(const Instance& instance, const Surface& surface)
    : instance_(instance), surface_(surface)
{
    uint32_t physDeviceCount(0);
    vkEnumeratePhysicalDevices(VkInstance(instance_), &physDeviceCount, nullptr);
    neko_assert(physDeviceCount != 0, "No GPU found!")
#ifdef VALIDATION_LAYERS
    std::ostringstream oss1;
    oss1 << "Found " << physDeviceCount << " GPUs:\n";
    logDebug(oss1.str());
#endif

    std::vector<VkPhysicalDevice> physDevices(physDeviceCount);
    const VkResult res = vkEnumeratePhysicalDevices(VkInstance(instance_), &physDeviceCount, physDevices.data());
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

        queueFamilyIndices_ = FindQueueFamilies(gpu, VkSurfaceKHR(surface_));
        if (IsDeviceSuitable(gpu))
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
    uint32_t familyQueueCount(0);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu_, &familyQueueCount, nullptr);
    neko_assert(familyQueueCount != 0, "Device has no family of queues associated with it")

    // Extract the properties of all the queue families
    std::vector<VkQueueFamilyProperties> queueProperties(familyQueueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu_, &familyQueueCount, queueProperties.data());

    // Make sure the family of commands contains an option to issue graphical commands.
    uint32_t queueNodeIndex = INVALID_INDEX;
    for (uint32_t i = 0; i < familyQueueCount; i++)
    {
        if (queueProperties[i].queueCount > 0 && queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueNodeIndex = i;
            break;
        }
    }

    neko_assert(queueNodeIndex != INVALID_INDEX,
            "Unable to find a queue command family that accepts graphics commands")

    PhysicalDeviceLocator::provide(this);
}

bool PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice const& gpu) const
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(gpu, &deviceProperties);
    vkGetPhysicalDeviceFeatures(gpu, &deviceFeatures);

    const bool extensionsSupported = CheckDeviceExtensionSupport(gpu);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(gpu, VkSurfaceKHR(surface_));
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(gpu, &supportedFeatures);

    return queueFamilyIndices_.IsComplete() && extensionsSupported && swapChainAdequate &&
           supportedFeatures.samplerAnisotropy &&
           deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
}
}