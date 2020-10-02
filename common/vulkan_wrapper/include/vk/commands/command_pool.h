#pragma once
#include "vk/vulkan_include.h"
#include "vk/core/logical_device.h"

namespace neko::vk
{
class CommandPool
{
public:
	CommandPool(const PhysicalDevice& gpu, const LogicalDevice& device);
	~CommandPool();
	
    explicit operator const VkCommandPool &() const { return commandPool_; }
    [[nodiscard]] const VkCommandPool& GetCommandPool() const { return commandPool_; }
	
private:
	const PhysicalDevice& gpu_;
	const LogicalDevice& device_;
	
	VkCommandPool commandPool_;
};
}
