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
#include "mathematics/hash.h"

#include "vk/buffers/storage_buffer.h"
#include "vk/shaders/uniform_block.h"

namespace neko::vk
{
class StorageHandle
{
public:
	explicit StorageHandle(bool multiPipeline = false);
	StorageHandle(const UniformBlock& uniformBlock, bool multiPipeline = false);

	void Destroy() const;

	void Push(const void* data, std::size_t size);

	template<typename T>
	void Push(const T& object, const std::size_t offset, const std::size_t size)
	{
		if (!uniformBlock_) return;

		if (memcmp(&arbitraryStorageData_[0] + offset, &object, size) != 0)
		{
			memcpy(&arbitraryStorageData_[0] + offset, &object, size);
			handleStatus_ = Buffer::Status::CHANGED;
		}
	}

	template<typename T>
	void Push(const StringHash& uniformHash, const T& object, const std::size_t size = 0)
	{
		if (!uniformBlock_) return;

		const Uniform& uniform = uniformBlock_->get().GetUniform(uniformHash);
		std::size_t realSize   = size;
		if (realSize == 0)
			realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform.GetSize()));

		Push(object, static_cast<std::size_t>(uniform.GetOffset()), realSize);
	}

	bool Update(const UniformBlock& uniformBlock);

	[[nodiscard]] const StorageBuffer& GetUniformBuffer() const { return *storageBuffer_; }

private:
	bool multiPipeline_;

	std::optional_const_ref<UniformBlock> uniformBlock_ = std::nullopt;
	VkDeviceSize size_                                  = 0;
	std::vector<char> arbitraryStorageData_;
	std::unique_ptr<StorageBuffer> storageBuffer_ = nullptr;

	Buffer::Status handleStatus_ = Buffer::Status::NORMAL;
};
}    // namespace neko::vk