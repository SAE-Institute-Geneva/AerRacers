#include "vk/vk_resources.h"

namespace neko::vk
{
Buffer::Buffer(const VkDeviceSize size,
	const VkBufferUsageFlags usage,
	const VkMemoryPropertyFlags properties,
	const void* data)
   : size_(size)
{
	const VkResources* vkObj                     = VkResources::Inst;
	const QueueFamilyIndices& queueFamilyIndices = vkObj->gpu.GetQueueFamilyIndices();
	std::array<std::uint32_t, 2> queueFamilies = {
		queueFamilyIndices.graphicsFamily, queueFamilyIndices.presentFamily,
		//computeFamily
	};

	// Create the buffer handle.
	VkBufferCreateInfo bufferInfo    = {};
	bufferInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size                  = size;
	bufferInfo.usage                 = usage;
	bufferInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.queueFamilyIndexCount = static_cast<std::uint32_t>(queueFamilies.size());
	bufferInfo.pQueueFamilyIndices   = queueFamilies.data();

	VkResult res = vkCreateBuffer(vkObj->device, &bufferInfo, nullptr, &buffer_);
	vkCheckError(res, "Failed to create vertex buffer!");

	// Create the memory backing up the buffer handle.
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vkObj->device, buffer_, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize       = memRequirements.size;
	allocInfo.memoryTypeIndex      = FindMemoryType(memRequirements.memoryTypeBits, properties);

	//TODO Stop allocating for every new buffer.
	res = vkAllocateMemory(vkObj->device, &allocInfo, nullptr, &memory_);
	vkCheckError(res, "Failed to allocate buffer memory!");

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data.
	if (data)
	{
		char* dataPtr;
		MapMemory(&dataPtr);
		std::memcpy(dataPtr, data, size);

		// If host coherency hasn't been requested, do a manual flush to make writes visible.
		if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange mappedMemoryRange = {};
			mappedMemoryRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedMemoryRange.memory              = memory_;
			mappedMemoryRange.offset              = 0;
			mappedMemoryRange.size                = size;
			vkFlushMappedMemoryRanges(vkObj->device, 1, &mappedMemoryRange);
		}

		UnmapMemory();
	}

	res = vkBindBufferMemory(vkObj->device, buffer_, memory_, 0);
	vkCheckError(res, "Failed to bind buffer memory!");
}

void Buffer::Destroy() const
{
	const LogicalDevice& device = VkResources::Inst->device;
	vkDestroyBuffer(device, buffer_, nullptr);
	vkFreeMemory(device, memory_, nullptr);
}

void Buffer::MapMemory(char** dataPtr) const
{
	const LogicalDevice& device = VkResources::Inst->device;
	VkResult res = vkMapMemory(device, memory_, 0, size_, 0, reinterpret_cast<void**>(dataPtr));
	vkCheckError(res, "Failed to map buffer memory!");
}

void Buffer::UnmapMemory() const
{
	vkUnmapMemory(VkResources::Inst->device, memory_);
}

std::uint32_t Buffer::FindMemoryType(
	const std::uint32_t typeFilter, const VkMemoryPropertyFlags& properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(VkResources::Inst->gpu, &memProperties);

	for (std::uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if (typeFilter & 1 << i &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	neko_assert(false, "Failed to find suitable memory type!");
}

void Buffer::CreateBuffer(const VkDevice& device,
	const VkDeviceSize& size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer buffer,
	VkDeviceMemory bufferMemory)
{
	VkBufferCreateInfo bufferInfo {};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = size;
	bufferInfo.usage       = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult res = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
	vkCheckError(res, "Failed to create vertex buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo {};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	res = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
	vkCheckError(res, "Failed to allocate buffer memory!");

	res = vkBindBufferMemory(device, buffer, bufferMemory, 0);
	vkCheckError(res, "Failed to bind buffer memory!");
}

[[maybe_unused]] void Buffer::CopyBuffer(
	VkBuffer srcBuffer, VkBuffer dstBuffer, const VkDeviceSize size)
{
	CommandBuffer commandBuffer(true);

	VkBufferCopy copyRegion {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	commandBuffer.SubmitIdle();
}
}    // namespace neko::vk
