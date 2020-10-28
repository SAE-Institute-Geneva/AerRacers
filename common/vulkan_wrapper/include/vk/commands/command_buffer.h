#pragma once
#include <thread>

#include "vk/vulkan_include.h"

namespace neko::vk
{
//TODO put some multithreading there
class CommandBuffer
{
public:
    void Init(bool begin = true,
              VkQueueFlagBits queueType = VK_QUEUE_GRAPHICS_BIT,
              VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    void Destroy();

    void Begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    void End();

    void SubmitIdle();
    void Submit(const VkSemaphore& waitSemaphore = nullptr,
            const VkSemaphore& signalSemaphore = nullptr,
            VkFence fence = nullptr);

    [[nodiscard]] bool IsRunning() const { return running_; }
    explicit operator const VkCommandBuffer &() const { return commandBuffer_; }
    [[nodiscard]] const VkCommandBuffer& GetCommandBuffer() const { return commandBuffer_; }
private:
    VkQueueFlagBits queueType_{};
    VkCommandBuffer commandBuffer_ = VK_NULL_HANDLE;

    bool running_ = false;

    [[nodiscard]] VkQueue GetQueue() const;

    //std::thread::id threadId_;
};
}