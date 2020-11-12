#pragma once
#include "vk/vulkan_include.h"

namespace neko::vk
{
class LogicalDevice final
{
public:
    explicit LogicalDevice() = default;

    void Init();
    void Destroy() const;

    explicit operator const VkDevice &() const { return device_; }
    [[nodiscard]] const VkDevice& GetLogicalDevice() const { return device_; }
    
    [[nodiscard]] const VkQueue& GetGraphicsQueue() const { return graphicsQueue_; }
    [[nodiscard]] const VkQueue& GetPresentQueue() const { return presentQueue_; }

private:
    VkDevice device_{};
    VkQueue graphicsQueue_{};
    VkQueue presentQueue_{};
};
}