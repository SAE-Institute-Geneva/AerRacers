#pragma once
#include "vk/core/logical_device.h"

namespace neko::vk
{
class Buffer
{
public:
    explicit operator const VkBuffer &() const { return buffer_; }
    explicit operator const VkDeviceMemory &() const { return memory_; }

    [[nodiscard]] const VkBuffer& GetBuffer() const { return buffer_; }
    [[nodiscard]] const VkDeviceMemory& GetBufferMemory() const { return memory_; }

    void Destroy()
    {
        vkDestroyBuffer(VkDevice(device_), buffer_, nullptr);
        vkFreeMemory(VkDevice(device_), memory_, nullptr);
    }

protected:
    const LogicalDevice& device_;

    explicit Buffer(const LogicalDevice& device) : device_(device) {}

    VkBuffer buffer_{};
    VkDeviceMemory memory_{};
};
}