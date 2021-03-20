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
#include "vk/buffers/push_handle.h"
#include "vk/buffers/storage_handle.h"
#include "vk/buffers/uniform_handle.h"
#include "vk/descriptors/descriptor_set.h"

namespace neko::vk
{
class DescriptorHandle
{
public:
	DescriptorHandle() = default;
	explicit DescriptorHandle(const Pipeline& pipeline);
	void Destroy() const;

	template <typename T>
    static T* AsPtr(T& obj) { return &obj; }

    template <typename T>
    static T* AsPtr(T* obj) { return obj; }

    template <typename T>
    static T* AsPtr(const std::unique_ptr<T>& obj) { return obj.get(); }

	void PushDescriptorData(const Material::PushDataContainer& dataContainer);

	template<typename T>
	void Push(const StringHash& descriptorHash, const T& descriptor)
	{
		if (!shader_) { return; }

		// Finds the local value given to the descriptor name.
		auto it = descriptor_.find(descriptorHash);
		if (it != descriptor_.end())
		{
			if (it->second.descriptor == AsPtr(descriptor)) { return; }

			descriptor_.erase(it);
		}

		// Only non-null descriptors can be mapped.
		if (!AsPtr(descriptor)) return;

		// When adding the descriptor find the location in the shader.
		auto location = shader_->get().GetDescriptorLocation(descriptorHash);
		if (location == INVALID_INDEX) return;

		// Adds the new descriptor value.
		auto descriptorType  = shader_->get().GetDescriptorType(location);
		auto writeDescriptor = AsPtr(descriptor)->GetWriteDescriptor(location, descriptorType);
		descriptor_.emplace(descriptorHash,
			DescriptorValue {AsPtr(descriptor), std::move(writeDescriptor), location});
		changed_ = true;
	}

	template<typename T>
	void Push(const StringHash& descriptorHash,
		const T& descriptor,
		const WriteDescriptorSet& writeDescriptorSet)
	{
		if (!shader_) return;

		const auto it = descriptor_.find(descriptorHash);
		if (it != descriptor_.end()) descriptor_.erase(it);

		const auto& location = shader_->get().GetDescriptorLocation(descriptorHash);
		descriptor_.emplace(
			descriptorHash, DescriptorValue {AsPtr(descriptor), writeDescriptorSet, location});
		changed_ = true;
	}

	void Push(const StringHash& descriptorHash, UniformHandle& uniformHandle);
	void Push(const StringHash& descriptorHash, StorageHandle& storageHandle);
	void Push(const StringHash& descriptorHash, PushHandle& pushHandle);

	bool Update(const Pipeline& pipeline);

	void BindDescriptor(const CommandBuffer& commandBuffer, const Pipeline& pipeline) const;

	[[nodiscard]] const DescriptorSet& GetDescriptorSet() const { return *descriptorSet_; }

private:
    struct DescriptorValue
	{
        const IDescriptor* descriptor;
        WriteDescriptorSet writeDescriptor;
        std::uint32_t location;
    };

    std::optional_const_ref<Shader> shader_ = std::nullopt;

    bool pushDescriptor_ = false;
    bool changed_ = false;

    std::optional<DescriptorSet> descriptorSet_ = std::nullopt;
    std::map<StringHash, DescriptorValue> descriptor_;
    std::vector<VkWriteDescriptorSet> writeDescriptorSets_;
};
}