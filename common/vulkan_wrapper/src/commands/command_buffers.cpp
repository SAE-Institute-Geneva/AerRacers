#include "vk/commands/command_buffers.h"

#include <array>

namespace neko::vk
{
CommandBuffers::CommandBuffers(const LogicalDevice& device, const CommandPool& commandPool)
	: device_(device), commandPool_(commandPool)
{}

void CommandBuffers::Init(
        const Swapchain& swapchain,
        const RenderPass& renderPass,
        const GraphicsPipeline& graphicsPipeline,
        const Framebuffers& framebuffers,
        const VertexBuffer& vertexBuffer,
        const IndexBuffer& indexBuffer,
        const DescriptorSets& descriptorSets)
{
    commandBuffers_.resize(framebuffers.GetFramebuffersCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = VkCommandPool(commandPool_);
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

    VkResult res = vkAllocateCommandBuffers(VkDevice(device_), &allocInfo, commandBuffers_.data());
    neko_assert(res == VK_SUCCESS, "Failed to allocate command buffers!")

    const auto& framebuffer = framebuffers.GetFramebuffers();
    for (size_t i = 0; i < commandBuffers_.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        res = vkBeginCommandBuffer(commandBuffers_[i], &beginInfo);
        neko_assert(res == VK_SUCCESS, "Failed to begin recording command buffer!")

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = VkRenderPass(renderPass);
        renderPassInfo.framebuffer = framebuffer[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain.GetExtent();

        std::array<VkClearValue, 1> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        //clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers_[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, VkPipeline(graphicsPipeline));

        VkBuffer vertexBuffers[] = {VkBuffer(vertexBuffer)};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffers_[i], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers_[i], VkBuffer(indexBuffer), 0, VK_INDEX_TYPE_UINT16);

        const auto& pipelineLayout = graphicsPipeline.GetLayout();
        vkCmdBindDescriptorSets(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout, 0, 1, &descriptorSets[i],0, nullptr);

        vkCmdDrawIndexed(commandBuffers_[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        vkCmdEndRenderPass(commandBuffers_[i]);

        res = vkEndCommandBuffer(commandBuffers_[i]);
        neko_assert(res == VK_SUCCESS, "Failed to record command buffer!")
    }
}

void CommandBuffers::Destroy()
{
    vkFreeCommandBuffers(VkDevice(device_), VkCommandPool(commandPool_),
                         static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
}
}
