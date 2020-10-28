#pragma once
#include "mathematics/matrix.h"

#include "vk/commands/command_buffer.h"
#include "vk/pipelines/pipeline.h"

namespace neko::vk
{
class WriteDescriptorSet {
public:
    WriteDescriptorSet(
            VkWriteDescriptorSet writeDescriptorSet,
            VkDescriptorImageInfo imageInfo);

    WriteDescriptorSet(
            VkWriteDescriptorSet writeDescriptorSet,
            VkDescriptorBufferInfo bufferInfo);

    VkWriteDescriptorSet GetWriteDescriptorSet();

private:
    VkWriteDescriptorSet writeDescriptorSet_{};
    VkDescriptorImageInfo imageInfo_{};
    VkDescriptorBufferInfo bufferInfo_{};
};

class IDescriptor
{
public:
    IDescriptor() = default;
    virtual ~IDescriptor() = default;

    [[nodiscard]] virtual WriteDescriptorSet GetWriteDescriptor(
            uint32_t binding, VkDescriptorType descriptorType) const = 0;
};

class DescriptorSet
{
public:
    explicit DescriptorSet(const Pipeline& pipeline);

    static void Update(const std::vector<VkWriteDescriptorSet>& descriptors);
    void Destroy() const;

    void BindDescriptor(const CommandBuffer& commandBuffer) const;

    explicit operator const VkDescriptorSet &() const { return descriptorSet_; }
    [[nodiscard]] const VkDescriptorSet& GetDescriptorSet() const { return descriptorSet_; }

private:
    VkPipelineLayout pipelineLayout_{};
    VkPipelineBindPoint pipelineBindPoint_{};
    VkDescriptorPool descriptorPool_{};
    VkDescriptorSet descriptorSet_{};
};
}