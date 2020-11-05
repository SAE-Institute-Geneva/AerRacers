#pragma once
#include <map>
#include <memory>

#include "optional.hpp"

#include "vk/buffers/push_handle.h"
#include "vk/buffers/storage_handle.h"
#include "vk/descriptors/descriptor_set.h"
#include "vk/pipelines/pipeline.h"
#include "vk/shaders/shader.h"

namespace neko::vk
{
class PushHandle;
class DescriptorHandle
{
public:
    DescriptorHandle() = default;
    explicit DescriptorHandle(const Pipeline& pipeline);
    void Destroy() const;
	
    DescriptorHandle(const DescriptorHandle& other);
    DescriptorHandle(DescriptorHandle&& other) noexcept;

    DescriptorHandle& operator=(const DescriptorHandle& other);
    DescriptorHandle& operator=(DescriptorHandle&& other) noexcept;

    template <typename T>
    static T* AsPtr(T& obj) { return &obj; }

    template <typename T>
    static T* AsPtr(T* obj) { return obj; }

    template <typename T>
    static T* AsPtr(const std::unique_ptr<T>& obj) { return obj.get(); }

    //void PushDescriptorData(const Material::PushDataContainer& dataContainer);

    template <typename T>
    void Push(const XXH64_hash_t descriptorHash, const T& descriptor)
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
        if (AsPtr(descriptor) == nullptr) return;

        // When adding the descriptor find the location in the shader.
        auto location = shader_->GetDescriptorLocation(descriptorHash);

        if(location == INVALID_INDEX) return;

        auto descriptorType = shader_->GetDescriptorType(location);

        // Adds the new descriptor value.
        auto writeDescriptor = AsPtr(descriptor)->GetWriteDescriptor(
                location, descriptorType);
        descriptor_.emplace(
                descriptorHash,
                DescriptorValue{
                        AsPtr(descriptor),
                        std::move(writeDescriptor),
                        location
                });
        changed_ = true;
    }

    template <typename T>
    void Push(const XXH64_hash_t descriptorHash,
            const T& descriptor,
            WriteDescriptorSet writeDescriptorSet)
    {
        if (!shader_) return;

        const auto it = descriptor_.find(descriptorHash);

        if (it != descriptor_.end()) descriptor_.erase(it);

        const auto& location = shader_->GetDescriptorLocation(descriptorHash);

        descriptor_.emplace(
                descriptorHash,
                DescriptorValue{
                        AsPtr(descriptor),
                        writeDescriptorSet,
                        location
                });
        changed_ = true;
    }

    void Push(XXH64_hash_t descriptorHash, UniformHandle& uniformHandle);
    void Push(XXH64_hash_t descriptorHash, StorageHandle& storageHandle);
    void Push(XXH64_hash_t descriptorHash, PushHandle& pushHandle);

    bool Update(const Pipeline& pipeline);

    void BindDescriptor(const CommandBuffer& commandBuffer, const Pipeline& pipeline) const;

    [[nodiscard]] const DescriptorSet& GetDescriptorSet() const { return *descriptorSet_; }

private:
    struct DescriptorValue
	{
        const IDescriptor* descriptor;
        WriteDescriptorSet writeDescriptor;
        uint32_t location;
    };

    std::experimental::optional<const Shader&> shader_ = std::experimental::nullopt;

    bool pushDescriptor_ = false;
    bool changed_ = false;
    //TODO Remove the unique_ptr
    std::unique_ptr<DescriptorSet> descriptorSet_ = nullptr;

    std::map<XXH64_hash_t, DescriptorValue> descriptor_;
    std::vector<VkWriteDescriptorSet> writeDescriptorSets_;
};
}