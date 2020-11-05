#pragma once
#include "vk/pipelines/pipeline.h"

namespace neko::vk
{
class CommandBuffer;
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