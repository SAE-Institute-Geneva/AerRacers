#include "vk/buffers/buffer.h"
#include "vk/graphics.h"

namespace neko::vk
{
Buffer::Buffer(const VkDeviceSize size,
               const VkBufferUsageFlags usage,
               const VkMemoryPropertyFlags properties,
               const void* data)
               : size_(size)
{
    const auto& vkObj = VkObjectsLocator::get();
    const auto queueFamilyIndices = vkObj.gpu.GetQueueFamilyIndices();

    std::array<std::uint32_t, 2> queueFamilies = {
            queueFamilyIndices.graphicsFamily,
            queueFamilyIndices.presentFamily,
            //computeFamily
    };

    // Create the buffer handle.
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = static_cast<std::uint32_t>(queueFamilies.size());
    bufferInfo.pQueueFamilyIndices = queueFamilies.data();

    VkResult res = vkCreateBuffer(VkDevice(vkObj.device), &bufferInfo, nullptr, &buffer_);
    neko_assert(res == VK_SUCCESS, "Failed to create vertex buffer!")

    // Create the memory backing up the buffer handle.
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VkDevice(vkObj.device), buffer_, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    //TODO Stop allocating for every new buffer.
    res = vkAllocateMemory(VkDevice(vkObj.device), &allocInfo, nullptr, &memory_);
    neko_assert(res == VK_SUCCESS, "Failed to allocate vertex buffer memory!")

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
            mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedMemoryRange.memory = memory_;
            mappedMemoryRange.offset = 0;
            mappedMemoryRange.size = size;
            vkFlushMappedMemoryRanges(VkDevice(vkObj.device), 1, &mappedMemoryRange);
        }

        UnmapMemory();
    }

    res = vkBindBufferMemory(VkDevice(vkObj.device), buffer_, memory_, 0);
    neko_assert(res == VK_SUCCESS, "Failed to bind buffer memory!")
}

void Buffer::Destroy() const
{
    const auto& vkObj = VkObjectsLocator::get();
	
	vkDestroyBuffer(VkDevice(vkObj.device), buffer_, nullptr);
	vkFreeMemory(VkDevice(vkObj.device), memory_, nullptr);
}

void Buffer::MapMemory(char** dataPtr) const
{
    const auto& device = VkDevice(VkObjectsLocator::get().device);
    const VkResult res =
            vkMapMemory(device, memory_, 0, size_, 0, reinterpret_cast<void**>(dataPtr));
    neko_assert(res == VK_SUCCESS, "Failed to map buffer memory!")
}

void Buffer::UnmapMemory() const
{
    const auto& device = VkDevice(VkObjectsLocator::get().device);
    vkUnmapMemory(device, memory_);
}

std::uint32_t Buffer::FindMemoryType(
	const std::uint32_t typeFilter, 
	const VkMemoryPropertyFlags& properties)
{
    const auto& vkObj = VkObjectsLocator::get();

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice(vkObj.gpu), &memProperties);

	for (std::uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if (typeFilter & 1 << i && (memProperties.memoryTypes[i].propertyFlags &
			properties) == properties)
			return i;

	neko_assert(false, "Failed to find suitable memory type!")
}

void Buffer::CreateBuffer(
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

	VkResult res = vkCreateBuffer(VkDevice(device), &bufferInfo, nullptr, &buffer);
	neko_assert(res == VK_SUCCESS, "Failed to create vertex buffer!")

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(VkDevice(device), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	res = vkAllocateMemory(VkDevice(device), &allocInfo, nullptr, &bufferMemory);
	neko_assert(res == VK_SUCCESS, "Failed to allocate vertex buffer memory!")

	vkBindBufferMemory(VkDevice(device), buffer, bufferMemory, 0);
}

VkCommandBuffer Buffer::BeginSingleTimeCommands(const VkDevice& device, const VkCommandPool& commandPool)
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

void Buffer::EndSingleTimeCommands(
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

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, {});
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void Buffer::CopyBuffer(const VkQueue& graphicsQueue, const VkDevice& device,
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
