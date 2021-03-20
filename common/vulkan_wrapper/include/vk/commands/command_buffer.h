#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/vk_include.h"

namespace neko::vk
{
constexpr std::uint64_t kDefaultFenceTimeout = 100000000000;
constexpr VkPipelineStageFlags kSubmitPipelineStages =
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

class CommandBuffer
{
public:
	CommandBuffer(bool begin,
		VkQueueFlagBits queueType        = VK_QUEUE_GRAPHICS_BIT,
		VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	operator const VkCommandBuffer&() const { return commandBuffer_; }

	void Init(bool begin                 = true,
		VkQueueFlagBits queueType        = VK_QUEUE_GRAPHICS_BIT,
		VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	void Destroy();

	void Begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	void End();

	void SubmitIdle(bool destroy = true);
	void Submit(
		VkSemaphore waitSemaphore = {}, VkSemaphore signalSemaphore       = {},
		VkFence fence                            = {});

	[[nodiscard]] bool IsRunning() const { return running_; }

	// Put an image memory barrier for setting an image layout on the sub resource into the given command buffer
	void SetImageLayout(VkImage image,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkImageSubresourceRange subresourceRange,
		VkPipelineStageFlags srcMask      = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dstMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

	// Uses a fixed sub resource layout with first mip level and layer
	void SetImageLayout(VkImage image,
		VkImageAspectFlags aspectMask,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkPipelineStageFlags srcMask      = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dstMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

private:
	VkQueueFlagBits queueType_ {};
	VkCommandBuffer commandBuffer_ {};

	bool running_ = false;

	[[nodiscard]] VkQueue GetQueue() const;
};
}    // namespace neko::vk