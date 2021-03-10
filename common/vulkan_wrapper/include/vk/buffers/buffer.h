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
class Buffer
{
public:
	enum class Status : std::uint8_t
	{
		RESET = 0,
		CHANGED,
		NORMAL
	};

	Buffer() = default;
	Buffer(VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		const void* data = nullptr);

	operator const VkBuffer*() const { return &buffer_; }
	operator const VkBuffer&() const { return buffer_; }

	virtual void Destroy() const;

	void MapMemory(char** dataPtr) const;
	void UnmapMemory() const;

	[[nodiscard]] VkDeviceSize GetSize() const { return size_; }
	[[nodiscard]] const VkBuffer& GetBuffer() const { return buffer_; }
	[[nodiscard]] const VkDeviceMemory& GetBufferMemory() const { return memory_; }

	static std::uint32_t FindMemoryType(
		std::uint32_t typeFilter, const VkMemoryPropertyFlags& properties);

	static void CreateBuffer(const VkDevice& device,
		const VkDeviceSize& size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer buffer,
		VkDeviceMemory bufferMemory);

	[[maybe_unused]] static void CopyBuffer(
		VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

protected:
	VkDeviceSize size_ = 0;
	VkBuffer buffer_ {};
	VkDeviceMemory memory_ {};
};
}    // namespace neko::vk