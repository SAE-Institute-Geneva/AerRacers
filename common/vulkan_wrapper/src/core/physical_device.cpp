#include "vk/core/physical_device.h"
#include "vk/vk_utilities.h"

namespace neko::vk
{
void PhysicalDevice::Init(const Instance& instance, const Surface& surface)
{
    uint32_t physDeviceCount(0);
    vkEnumeratePhysicalDevices(VkInstance(instance), &physDeviceCount, nullptr);
    neko_assert(physDeviceCount != 0, "No GPU found!")
#ifdef VALIDATION_LAYERS
    std::ostringstream oss1;
    oss1 << "Found " << physDeviceCount << " GPUs:\n";
    logDebug(oss1.str());
#endif

    std::vector<VkPhysicalDevice> physDevices(physDeviceCount);
    const VkResult res = vkEnumeratePhysicalDevices(VkInstance(instance), &physDeviceCount, physDevices.data());
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

        queueFamilyIndices_ = FindQueueFamilies(gpu, VkSurfaceKHR(surface));
        if (IsDeviceSuitable(gpu, VkSurfaceKHR(surface), queueFamilyIndices_))
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
}
}