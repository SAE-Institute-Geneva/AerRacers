#pragma once
#include "vk/vulkan_include.h"

namespace neko::vk
{
class CommandPool
{
public:
	explicit CommandPool() = default;

	void Init();
	void Destroy() const;
	
    explicit operator const VkCommandPool &() const { return commandPool_; }
    [[nodiscard]] const VkCommandPool& GetCommandPool() const { return commandPool_; }
	
private:
	VkCommandPool commandPool_{};
};
}
