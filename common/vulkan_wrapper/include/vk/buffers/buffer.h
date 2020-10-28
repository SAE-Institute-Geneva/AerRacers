#pragma once
#include "vk/core/logical_device.h"

namespace neko::vk
{
class Buffer
{
public:
    enum class Status : uint8_t
    {
        RESET = 0,
        CHANGED,
        NORMAL
    };

    explicit Buffer() = default;
    Buffer(VkDeviceSize size,
           VkBufferUsageFlags usage,
           VkMemoryPropertyFlags properties,
           const void *data = nullptr);

    void MapMemory(char** dataPtr) const;
    void UnmapMemory() const;

    explicit operator const VkBuffer &() const { return buffer_; }
    explicit operator const VkDeviceMemory &() const { return memory_; }

    [[nodiscard]] VkDeviceSize GetSize() const { return size_; }
    [[nodiscard]] const VkBuffer& GetBuffer() const { return buffer_; }
    [[nodiscard]] const VkDeviceMemory& GetBufferMemory() const { return memory_; }

    void Destroy() const;


    static uint32_t FindMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags& properties);

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

protected:
    VkDeviceSize size_ = 0;
    VkBuffer buffer_{};
    VkDeviceMemory memory_{};
};
}