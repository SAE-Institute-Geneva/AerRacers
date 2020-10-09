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

    void Destroy(const LogicalDevice& device) const;

protected:
    explicit Buffer() = default;

    VkBuffer buffer_{};
    VkDeviceMemory memory_{};
};

static uint32_t FindMemoryType(
    const VkPhysicalDevice& gpu, 
    uint32_t typeFilter,
    const VkMemoryPropertyFlags& properties);

static void CreateBuffer(
    const VkPhysicalDevice& gpu,
    const VkDevice& device,
    const VkDeviceSize& size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory);

static VkCommandBuffer BeginSingleTimeCommands(const VkDevice& device, const VkCommandPool& commandPool);
static void EndSingleTimeCommands(
	const VkQueue& graphicsQueue,
	const VkDevice& device,
	const VkCommandPool& commandPool,
	const VkCommandBuffer& commandBuffer);

static void CopyBuffer(
	const VkQueue& graphicsQueue,
	const VkDevice& device,
	const VkCommandPool& commandPool,
	const VkBuffer& srcBuffer,
	const VkBuffer& dstBuffer,
	VkDeviceSize size);
}