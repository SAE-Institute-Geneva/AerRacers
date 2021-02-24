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
	queueType_        = queueType;
	VkResources* vkObj = VkResources::Inst;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool                 = vkObj->GetCurrentCmdPool();
	allocInfo.level                       = bufferLevel;
	allocInfo.commandBufferCount          = 1;

	//threadId_ = vkObj.commandPool.GetThreadId();

	const VkResult res =
		vkAllocateCommandBuffers(VkDevice(vkObj->device), &allocInfo, &commandBuffer_);
	neko_assert(res == VK_SUCCESS, "Could not allocate memory for command buffer!")

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

void CommandBuffer::SubmitIdle(bool destroy)
{
	const auto queueSelected = GetQueue();
	const VkResources* vkObj        = VkResources::Inst;

	if (running_) End();

	VkSubmitInfo submitInfo       = {};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer_;

	VkFenceCreateInfo fenceInfo {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = 0;

	VkFence fence;
	VkResult res = vkCreateFence(VkDevice(vkObj->device), &fenceInfo, nullptr, &fence);
	neko_assert(res == VK_SUCCESS, "Could not create fence!")

	res = vkQueueSubmit(queueSelected, 1, &submitInfo, fence);
	neko_assert(res == VK_SUCCESS, "Could not submit command buffer to queue!")

	res = vkWaitForFences(VkDevice(vkObj->device), 1, &fence, VK_TRUE, kDefaultFenceTimeout);
	vkDestroyFence(VkDevice(vkObj->device), fence, nullptr);
	neko_assert(res == VK_SUCCESS, "Error while waiting for queue!")

	if (destroy) Destroy();
}

void CommandBuffer::Submit(
	const VkSemaphore& waitSemaphore, const VkSemaphore& signalSemaphore, VkFence fence)
{
	const VkResources* vkObj        = VkResources::Inst;
	const auto queueSelected = GetQueue();

	if (running_) End();

	VkSubmitInfo submitInfo       = {};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer_;

	if (waitSemaphore)
	{
		// Pipeline stages used to wait at for graphics queue submissions.
		static VkPipelineStageFlags submitPipelineStages =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		submitInfo.pWaitDstStageMask  = &submitPipelineStages;
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
		res = vkWaitForFences(
			VkDevice(vkObj->device), 1, &fence, VK_TRUE, std::numeric_limits<std::uint64_t>::max());
		neko_assert(res == VK_SUCCESS, "Error while waiting for fence!")

		res = vkResetFences(VkDevice(vkObj->device), 1, &fence);
		neko_assert(res == VK_SUCCESS, "Could not reset fence!")
	}

	res = vkQueueSubmit(queueSelected, 1, &submitInfo, fence);
	neko_assert(res == VK_SUCCESS, "Could not submit command buffer to queue!")
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
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask)
{
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask              = aspectMask;
	subresourceRange.baseMipLevel            = 0;
	subresourceRange.levelCount              = 1;
	subresourceRange.layerCount              = 1;
	SetImageLayout(
		image, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
}

void CommandBuffer::SetImageLayout(VkImage image,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkImageSubresourceRange subresourceRange,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask)
{
	// Create an image barrier object
	VkImageMemoryBarrier imageMemoryBarrier {};
	imageMemoryBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout           = oldImageLayout;
	imageMemoryBarrier.newLayout           = newImageLayout;
	imageMemoryBarrier.image               = image;
	imageMemoryBarrier.subresourceRange    = subresourceRange;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (oldImageLayout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			imageMemoryBarrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			// Image is preinitialized
			// Only valid as initial layout for linear images, preserves memory contents
			// Make sure host writes have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image is a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image is a depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image is read by a shader
			// Make sure any shader reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (newImageLayout)
	{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image will be used as a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image layout will be used as a depth/stencil attachment
			// Make sure any writes to depth/stencil buffer have been finished
			imageMemoryBarrier.dstAccessMask =
				imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (imageMemoryBarrier.srcAccessMask == 0)
			{
				imageMemoryBarrier.srcAccessMask =
					VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
	}

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(commandBuffer_,
		srcStageMask,
		dstStageMask,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&imageMemoryBarrier);
}
}    // namespace neko::vk