#include "vk/vk_resources.h"

namespace neko::vk
{
CommandBuffer::CommandBuffer(
	bool begin, VkQueueFlagBits queueType, VkCommandBufferLevel bufferLevel)
   : queueType_(queueType)
{
	Init(begin, queueType, bufferLevel);
}

void CommandBuffer::Init(bool begin, VkQueueFlagBits queueType, VkCommandBufferLevel bufferLevel)
{
	queueType_         = queueType;
	VkResources* vkObj = VkResources::Inst;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool                 = vkObj->GetCurrentCmdPool();
	allocInfo.level                       = bufferLevel;
	allocInfo.commandBufferCount          = 1;

	const VkResult res = vkAllocateCommandBuffers(vkObj->device, &allocInfo, &commandBuffer_);
	vkCheckError(res, "Could not allocate memory for command buffer!");

	if (begin) Begin();
}

void CommandBuffer::Destroy()
{
	VkResources* vkObj = VkResources::Inst;
	vkFreeCommandBuffers(vkObj->device, vkObj->GetCurrentCmdPool(), 1, &commandBuffer_);
}

void CommandBuffer::Begin(VkCommandBufferUsageFlags usage)
{
	if (running_) return;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags                    = usage;

	const VkResult res = vkBeginCommandBuffer(commandBuffer_, &beginInfo);
	vkCheckError(res, "Could not begin to write to the command buffer!");

	running_ = true;
}

void CommandBuffer::End()
{
	if (!running_) return;

	const VkResult res = vkEndCommandBuffer(commandBuffer_);
	vkCheckError(res, "Could not stop to write to the command buffer!");

	running_ = false;
}

void CommandBuffer::SubmitIdle(bool destroy)
{
	VkQueue selectedQueue    = GetQueue();
	const VkResources* vkObj = VkResources::Inst;

	if (running_) End();

	VkSubmitInfo submitInfo       = {};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer_;

	VkFenceCreateInfo fenceInfo {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = 0;

	VkFence fence;
	VkResult res = vkCreateFence(vkObj->device, &fenceInfo, nullptr, &fence);
	vkCheckError(res, "Could not create fence!");

	res = vkQueueSubmit(selectedQueue, 1, &submitInfo, fence);
	vkCheckError(res, "Could not submit command buffer to queue!");

	res = vkWaitForFences(vkObj->device, 1, &fence, VK_TRUE, kDefaultFenceTimeout);
	vkDestroyFence(vkObj->device, fence, nullptr);
	vkCheckError(res, "Error while waiting for queue!");

	if (destroy) Destroy();
}

void CommandBuffer::Submit(VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence)
{
	VkQueue selectedQueue    = GetQueue();
	const VkResources* vkObj = VkResources::Inst;

	if (running_) End();

	VkSubmitInfo submitInfo       = {};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer_;

	if (waitSemaphore)
	{
		// Pipeline stages used to wait at for graphics queue submissions.
		submitInfo.pWaitDstStageMask  = &kSubmitPipelineStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores    = &waitSemaphore;
	}

	if (signalSemaphore)
	{
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores    = &signalSemaphore;
	}

	VkResult res;
	if (fence)
	{
		res = vkWaitForFences(vkObj->device, 1, &fence, VK_TRUE, kDefaultFenceTimeout);
		vkCheckError(res, "Error while waiting for fence!");

		res = vkResetFences(vkObj->device, 1, &fence);
		vkCheckError(res, "Could not reset fence!");
	}

	res = vkQueueSubmit(selectedQueue, 1, &submitInfo, fence);
	vkCheckError(res, "Could not submit command buffer to queue!");
}

VkQueue CommandBuffer::GetQueue() const
{
	const VkResources* vkObj = VkResources::Inst;
	switch (queueType_)
	{
		case VK_QUEUE_GRAPHICS_BIT: return vkObj->device.GetGraphicsQueue();
		/*case VK_QUEUE_COMPUTE_BIT:
            return vkObj.device.GetComputeQueue();*/
		default: return nullptr;
	}
}

void CommandBuffer::SetImageLayout(VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkPipelineStageFlags srcMask,
	VkPipelineStageFlags dstMask)
{
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask              = aspectMask;
	subresourceRange.baseMipLevel            = 0;
	subresourceRange.levelCount              = 1;
	subresourceRange.layerCount              = 1;
	SetImageLayout(image, oldLayout, newLayout, subresourceRange, srcMask, dstMask);
}

void CommandBuffer::SetImageLayout(VkImage image,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkImageSubresourceRange subresourceRange,
	VkPipelineStageFlags srcMask,
	VkPipelineStageFlags dstMask)
{
	// Create an image barrier object
	VkImageMemoryBarrier barrier {};
	barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.oldLayout           = oldLayout;
	barrier.newLayout           = newLayout;
	barrier.image               = image;
	barrier.subresourceRange    = subresourceRange;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (oldLayout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			barrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			// Image is preinitialized
			// Only valid as initial layout for linear images, preserves memory contents
			// Make sure host writes have been finished
			barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image is a color attachment
			// Make sure any writes to the color buffer have been finished
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image is a depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source
			// Make sure any reads from the image have been finished
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image is read by a shader
			// Make sure any shader reads from the image have been finished
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (newLayout)
	{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from the image have been finished
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image will be used as a color attachment
			// Make sure any writes to the color buffer have been finished
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image layout will be used as a depth/stencil attachment
			// Make sure any writes to depth/stencil buffer have been finished
			barrier.dstAccessMask =
				barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (barrier.srcAccessMask == 0)
			{
				barrier.srcAccessMask =
					VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
	}

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(commandBuffer_, srcMask, dstMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}
}    // namespace neko::vk