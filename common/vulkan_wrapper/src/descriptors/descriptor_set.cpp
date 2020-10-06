#include "vk/descriptors/descriptor_set.h"

namespace neko::vk
{
DescriptorSets::DescriptorSets(const LogicalDevice& device) : device_(device)
{}

void DescriptorSets::InitLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    const VkResult res = vkCreateDescriptorSetLayout(VkDevice(device_), &layoutInfo, nullptr, &descriptorSetLayout_);
    neko_assert(res == VK_SUCCESS, "Failed to create descriptor set layout!")
}

void DescriptorSets::Init(const Swapchain& swapchain,
                          const std::vector<UniformBuffer>& uniformBuffers,
                          const DescriptorPool& descriptorPool)
{
    const size_t swapChainImagesCount = swapchain.GetImagesCount();
    std::vector<VkDescriptorSetLayout> layouts(swapChainImagesCount, descriptorSetLayout_);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = VkDescriptorPool(descriptorPool);
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImagesCount);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_.resize(swapChainImagesCount);
    const VkResult res = vkAllocateDescriptorSets(VkDevice(device_), &allocInfo, descriptorSets_.data());
    neko_assert(res == VK_SUCCESS, "Failed to allocate descriptor sets!")

    for (size_t i = 0; i < swapChainImagesCount; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = VkBuffer(uniformBuffers[i]);
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        /*VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = image_.view;
        imageInfo.sampler = image_.sampler;*/

        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets_[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        /*descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets_[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;*/

        vkUpdateDescriptorSets(VkDevice(device_), static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}

void DescriptorSets::Destroy()
{
    vkDestroyDescriptorSetLayout(VkDevice(device_), descriptorSetLayout_, nullptr);
}
}