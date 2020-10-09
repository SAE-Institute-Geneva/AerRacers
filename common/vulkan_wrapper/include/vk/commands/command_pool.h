#pragma once
#include "vk/vulkan_include.h"
#include "vk/core/logical_device.h"

namespace neko::vk
{
class CommandPool
{
public:
	explicit CommandPool() = default;

	void Init(const PhysicalDevice& gpu, const LogicalDevice& device);
	void Destroy(const LogicalDevice& device) const;
	
    explicit operator const VkCommandPool &() const { return commandPool_; }
    [[nodiscard]] const VkCommandPool& GetCommandPool() const { return commandPool_; }
	
private:
	VkCommandPool commandPool_{};
};
}
