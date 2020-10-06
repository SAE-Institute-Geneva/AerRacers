#pragma once
#include "vk/vulkan_include.h"
#include "vk/core/logical_device.h"

namespace neko::vk
{
class CommandPool
{
public:
	explicit CommandPool(const LogicalDevice& device);

	void Init(const PhysicalDevice& gpu);
	void Destroy();
	
    explicit operator const VkCommandPool &() const { return commandPool_; }
    [[nodiscard]] const VkCommandPool& GetCommandPool() const { return commandPool_; }
	
private:
	const LogicalDevice& device_;
	
	VkCommandPool commandPool_{};
};
}
