#include "vk/commands/command_buffer.h"
#include "vk/graphics.h"

namespace neko::vk
{
void CommandBuffer::Init(bool begin, VkQueueFlagBits queueType, VkCommandBufferLevel bufferLevel)
{
    queueType_ = queueType;
    const auto& vkObj = VkObjectsLocator::get();

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = VkCommandPool(*vkObj.commandPools);
    allocInfo.level = bufferLevel;
    allocInfo.commandBufferCount = 1;

    //threadId_ = vkObj.commandPool.GetThreadId();

    const VkResult res = vkAllocateCommandBuffers(VkDevice(vkObj.device), &allocInfo, &commandBuffer_);
    neko_assert(res == VK_SUCCESS, "Could not allocate memory for command buffer!")

    if (begin) Begin();
}

void CommandBuffer::Destroy()
{
    const auto& vkObj = VkObjectsLocator::get();
    vkFreeCommandBuffers(
            VkDevice(vkObj.device), VkCommandPool(*vkObj.commandPools), 1, &commandBuffer_);
}

void CommandBuffer::Begin(VkCommandBufferUsageFlags usage)
{
    if (running_) return;
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usage;

    const VkResult res = vkBeginCommandBuffer(commandBuffer_, &beginInfo);
    neko_assert(res == VK_SUCCESS, "Could not begin to write to the command buffer!")

    running_ = true;
}

void CommandBuffer::End()
{
    if (!running_) return;

    const VkResult res = vkEndCommandBuffer(commandBuffer_);
    neko_assert(res == VK_SUCCESS, "Could not stop to write to the command buffer!")

    running_ = false;
}

void CommandBuffer::SubmitIdle()
{
    const auto queueSelected = GetQueue();

    if (running_) End();

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer_;

    VkResult res = vkQueueSubmit(queueSelected, 1, &submitInfo, nullptr);
    neko_assert(res == VK_SUCCESS, "Could not submit command buffer to queue!")

    res = vkQueueWaitIdle(queueSelected);
    neko_assert(res == VK_SUCCESS, "Error while waiting for queue!")
}

void CommandBuffer::Submit(
    const VkSemaphore& waitSemaphore, 
    const VkSemaphore& signalSemaphore,
    VkFence fence)
{
    const auto& vkObj = VkObjectsLocator::get();
    const auto queueSelected = GetQueue();

    if (running_) End();

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer_;

    if (waitSemaphore)
    {
        // Pipeline stages used to wait at for graphics queue submissions.
        static VkPipelineStageFlags submitPipelineStages =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submitInfo.pWaitDstStageMask = &submitPipelineStages;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphore;
    }

    if (signalSemaphore)
    {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphore;
    }

    VkResult res;
    if (fence)
    {
        res = vkWaitForFences(VkDevice(vkObj.device), 1, &fence,
                        VK_TRUE, std::numeric_limits<std::uint64_t>::max());
        neko_assert(res == VK_SUCCESS, "Error while waiting for fence!")

        res = vkResetFences(VkDevice(vkObj.device), 1, &fence);
        neko_assert(res == VK_SUCCESS, "Could not reset fence!")
    }

    res = vkQueueSubmit(queueSelected, 1, &submitInfo, fence);
    neko_assert(res == VK_SUCCESS, "Could not submit command buffer to queue!")
}

VkQueue CommandBuffer::GetQueue() const
{
    const auto& vkObj = VkObjectsLocator::get();
    switch (queueType_)
    {
        case VK_QUEUE_GRAPHICS_BIT:
            return vkObj.device.GetGraphicsQueue();
        /*case VK_QUEUE_COMPUTE_BIT:
            return vkObj.device.GetComputeQueue();*/
        default:
            return nullptr;
    }
}
}