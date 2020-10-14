#include "vk/buffers/buffer.h"
#include "vk/graphics.h"

namespace neko::vk
{
void Buffer::Destroy() const
{
    const auto& vkObj = VkResourcesLocator::get();
	vkDestroyBuffer(VkDevice(vkObj.device), buffer_, nullptr);
	vkFreeMemory(VkDevice(vkObj.device), memory_, nullptr);
}

uint32_t FindMemoryType(
	const VkPhysicalDevice& gpu, 
	const uint32_t typeFilter,
	const VkMemoryPropertyFlags& properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if (typeFilter & 1 << i && (memProperties.memoryTypes[i].propertyFlags &
			properties) == properties)
			return i;

	neko_assert(false, "Failed to find suitable memory type!")
}

void CreateBuffer(
	const VkPhysicalDevice& gpu, 
	const VkDevice& device,
	const VkDeviceSize& size, 
	const VkBufferUsageFlags usage,
	const VkMemoryPropertyFlags properties, 
	VkBuffer& buffer,
	VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult res = vkCreateBuffer(VkDevice(device), &bufferInfo, nullptr,
	                              &buffer);
	neko_assert(res == VK_SUCCESS, "Failed to create vertex buffer!")

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(VkDevice(device), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(gpu, memRequirements.memoryTypeBits, properties);

	res = vkAllocateMemory(VkDevice(device), &allocInfo, nullptr,
	                       &bufferMemory);
	neko_assert(res == VK_SUCCESS, "Failed to allocate vertex buffer memory!")

	vkBindBufferMemory(VkDevice(device), buffer, bufferMemory, 0);
}

VkCommandBuffer BeginSingleTimeCommands(const VkDevice& device, const VkCommandPool& commandPool)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer{};
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void EndSingleTimeCommands(
	const VkQueue& graphicsQueue, 
	const VkDevice& device,
	const VkCommandPool& commandPool,
	const VkCommandBuffer& commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void CopyBuffer(const VkQueue& graphicsQueue, const VkDevice& device,
                const VkCommandPool& commandPool, const VkBuffer& srcBuffer,
                const VkBuffer& dstBuffer, const VkDeviceSize size)
{
	const VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(graphicsQueue, device, commandPool, commandBuffer);
}
}
