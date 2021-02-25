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
#include "vk/buffers/uniform_buffer.h"
#include "vk/material/material.h"

namespace neko::vk
{
class UniformHandle
{
public:
	explicit UniformHandle(bool multiPipeline = false);
	UniformHandle(const UniformBlock& uniformBlock, bool multiPipeline = false);
	~UniformHandle();

	void Destroy() const;

	UniformHandle(const UniformHandle& other);
	UniformHandle(UniformHandle&& other) noexcept;

	UniformHandle& operator=(const UniformHandle& other);
	UniformHandle& operator=(UniformHandle&& other) noexcept;

	void PushUniformData(const Material::PushDataContainer& dataContainer);

	template<typename T>
	void Push(const T& object, const std::size_t offset, const std::size_t size)
	{
		if (std::memcmp(&arbitraryUniformData_[0] + offset, &object, size) != 0)
		{
			std::memcpy(&arbitraryUniformData_[0] + offset, &object, size);
			handleStatus_ = Buffer::Status::CHANGED;
		}
	}

	template<typename T>
	void Push(const StringHash& uniformHash, const T& object, const std::size_t size = 0)
	{
		if (!uniformBlock_) return;

		const auto& uniform = uniformBlock_->get().GetUniform(uniformHash);

		auto realSize = size;
		if (realSize == 0)
			realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform.GetSize()));

		Push(object, static_cast<std::size_t>(uniform.GetOffset()), realSize);
	}

	bool Update(const UniformBlock& uniformBlock);

	[[nodiscard]] const UniformBuffer& GetUniformBuffer() const { return *uniformBuffer_; }

private:
	bool multiPipeline_;

	VkDeviceSize size_ = 0;
	std::vector<char> arbitraryUniformData_ {};

	std::optional_const_ref<UniformBlock> uniformBlock_ = std::nullopt;
	std::unique_ptr<UniformBuffer> uniformBuffer_       = nullptr;

	Buffer::Status handleStatus_ = Buffer::Status::NORMAL;
};
}    // namespace neko::vk
