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
#include "utils/std_utility.h"

#include "vk/pipelines/pipeline.h"

namespace neko::vk
{
class PushHandle
{
public:
	PushHandle() = default;
	explicit PushHandle(const UniformBlock& uniformBlock);

	template<typename T>
	void Push(const T& object, const std::size_t offset, const std::size_t size)
	{
		std::memcpy(&data_[0] + offset, &object, size);
	}

	template<typename T>
	void Push(const StringHash& uniformHash, const T& object, const std::size_t size = 0)
	{
		if (!uniformBlock_) return;

		const auto& uniform = uniformBlock_->get().GetUniform(uniformHash);

		auto realSize = size;
		if (realSize == 0)
		{
			realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform.GetSize()));
		}

		Push(object, static_cast<std::size_t>(uniform.GetOffset()), realSize);
	}

	bool Update(const UniformBlock& uniformBlock);

	void BindPush(const CommandBuffer& commandBuffer, const Pipeline& pipeline) const;

private:
	std::optional_const_ref<UniformBlock> uniformBlock_ = std::nullopt;
	std::vector<char> data_;
};
}    // namespace neko::vk