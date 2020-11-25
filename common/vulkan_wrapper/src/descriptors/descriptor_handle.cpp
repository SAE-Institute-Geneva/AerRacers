#include "vk/descriptors/descriptor_handle.h"

#include "vk/graphics.h"

namespace neko::vk
{
DescriptorHandle::DescriptorHandle(const Pipeline& pipeline)
        : shader_(pipeline.GetShader()),
          pushDescriptor_(pipeline.IsPushDescriptor()),
          changed_(true),
          descriptorSet_(std::make_unique<DescriptorSet>(pipeline)) {}

void DescriptorHandle::Destroy() const
{
	for (auto& descriptor : descriptor_)
		descriptor.second.descriptor->Destroy();
	if (descriptorSet_) descriptorSet_.get()->Destroy();
}

DescriptorHandle::DescriptorHandle(const DescriptorHandle& other)
        : shader_(other.shader_),
          pushDescriptor_(other.pushDescriptor_),
          changed_(other.changed_),
          descriptor_(other.descriptor_),
          writeDescriptorSets_(other.writeDescriptorSets_)
{
    if (other.descriptorSet_)
        descriptorSet_ = std::make_unique<DescriptorSet>(*other.descriptorSet_);
}

DescriptorHandle::DescriptorHandle(DescriptorHandle&& other) noexcept
        : shader_(other.shader_),
          pushDescriptor_(other.pushDescriptor_),
          changed_(other.changed_),
          descriptorSet_(std::move(other.descriptorSet_)),
          descriptor_(std::move(other.descriptor_)),
          writeDescriptorSets_(std::move(other.writeDescriptorSets_)) {}

DescriptorHandle& DescriptorHandle::operator=(const DescriptorHandle& other)
{
    if (this == &other)
        return *this;

    shader_ = other.shader_;
    pushDescriptor_ = other.pushDescriptor_;
    changed_ = other.changed_;
    if (other.descriptorSet_)
        descriptorSet_ = std::make_unique<DescriptorSet>(*other.descriptorSet_.get());

    descriptor_ = other.descriptor_;
    writeDescriptorSets_ = other.writeDescriptorSets_;
    return *this;
}

DescriptorHandle& DescriptorHandle::operator=(DescriptorHandle&& other) noexcept
{
    if (this == &other)
        return *this;

    shader_ = other.shader_;
    pushDescriptor_ = other.pushDescriptor_;
    changed_ = other.changed_;
    descriptorSet_ = std::move(other.descriptorSet_);
    descriptor_ = std::move(other.descriptor_);
    writeDescriptorSets_ = std::move(other.writeDescriptorSets_);
    return *this;
}

void DescriptorHandle::PushDescriptorData(const Material::PushDataContainer& dataContainer)
{
    for (const auto& data : dataContainer)
    {
        if (data.second.GetType() == MaterialExportData::Type::IMAGE_2D)
            Push(data.first, data.second.GetImage2d());
        //else
        //    Push(data.first, data.second.GetImageCube());
    }
}

void DescriptorHandle::Push(const XXH64_hash_t descriptorHash, UniformHandle& uniformHandle)
{
    if (!shader_) return;

    uniformHandle.Update(shader_->GetUniformBlock(descriptorHash));

    Push(descriptorHash, uniformHandle.GetUniformBuffer());
}

void DescriptorHandle::Push(const XXH64_hash_t descriptorHash, StorageHandle& storageHandle)
{
    if (!shader_) return;

    storageHandle.Update(shader_->GetUniformBlock(descriptorHash));
    Push(descriptorHash, storageHandle.GetUniformBuffer());
}

void DescriptorHandle::Push(const XXH64_hash_t descriptorHash, PushHandle& pushHandle)
{
    if (!shader_) return;

    pushHandle.Update(shader_->GetUniformBlock(descriptorHash));
}

bool DescriptorHandle::Update(const Pipeline& pipeline)
{
    //Check if the shader is the same as the one use in the pipeline
    if (!shader_ || &*shader_ != &pipeline.GetShader())
    {
        shader_.reset();
        shader_.emplace(pipeline.GetShader());
        pushDescriptor_ = pipeline.IsPushDescriptor();
        descriptor_.clear();
        writeDescriptorSets_.clear();

        if (!pushDescriptor_)
            descriptorSet_ = std::make_unique<DescriptorSet>(pipeline);

        changed_ = false;
        return false;
    }

    //If the descriptor has been updated, create writing descriptor
    if (changed_)
    {
        writeDescriptorSets_.clear();
        writeDescriptorSets_.reserve(descriptor_.size());

        for (auto& descriptor : descriptor_)
        {
            auto writeDescriptorSet = descriptor.second.writeDescriptor.GetWriteDescriptorSet();
            writeDescriptorSet.dstSet = VK_NULL_HANDLE;

            if (!pushDescriptor_)
                if (descriptorSet_)
                    writeDescriptorSet.dstSet = descriptorSet_->GetDescriptorSet();

            writeDescriptorSets_.emplace_back(writeDescriptorSet);
        }

        if (!pushDescriptor_) descriptorSet_->Update(writeDescriptorSets_);

        changed_ = false;
    }

    return true;
}

void DescriptorHandle::BindDescriptor(const CommandBuffer& commandBuffer, const Pipeline& pipeline) const
{
    if (pushDescriptor_)
    {
        const auto& vkObj = VkObjectsLocator::get();
        Instance::CmdPushDescriptorSetKhr(
                VkDevice(vkObj.device),
                VkCommandBuffer(commandBuffer),
                pipeline.GetPipelineBindPoint(),
                pipeline.GetPipelineLayout(), 0,
                static_cast<std::uint32_t>(writeDescriptorSets_.size()),
                writeDescriptorSets_.data());
    }
    else
    {
        descriptorSet_->BindDescriptor(commandBuffer);
    }
}
}
