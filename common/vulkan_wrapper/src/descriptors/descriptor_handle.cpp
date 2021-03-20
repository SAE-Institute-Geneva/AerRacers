#include "vk/vk_resources.h"

namespace neko::vk
{
DescriptorHandle::DescriptorHandle(const Pipeline& pipeline)
   : shader_(pipeline.GetShader()),
	 pushDescriptor_(pipeline.IsPushDescriptor()),
	 changed_(true),
	 descriptorSet_(pipeline)
{}

void DescriptorHandle::Destroy() const
{
	for (auto& descriptor : descriptor_) descriptor.second.descriptor->Destroy();
	if (descriptorSet_) descriptorSet_->Destroy();
}

void DescriptorHandle::PushDescriptorData(const Material::PushDataContainer& dataContainer)
{
	for (const auto& data : dataContainer)
	{
		if (data.second.GetType() == MaterialExportData::IMAGE_2D)
			Push(data.first, data.second.GetImage2d());
		//else
		//    Push(data.first, data.second.GetImageCube());
	}
}

void DescriptorHandle::Push(const StringHash& descriptorHash, UniformHandle& uniformHandle)
{
	if (!shader_) return;

	uniformHandle.Update(shader_->get().GetUniformBlock(descriptorHash));
	Push(descriptorHash, uniformHandle.GetUniformBuffer());
}

void DescriptorHandle::Push(const StringHash& descriptorHash, StorageHandle& storageHandle)
{
	if (!shader_) return;

	storageHandle.Update(shader_->get().GetUniformBlock(descriptorHash));
	Push(descriptorHash, storageHandle.GetUniformBuffer());
}

void DescriptorHandle::Push(const StringHash& descriptorHash, PushHandle& pushHandle)
{
	if (!shader_) return;

	pushHandle.Update(shader_->get().GetUniformBlock(descriptorHash));
}

bool DescriptorHandle::Update(const Pipeline& pipeline)
{
	//Check if the shader is the same as the one use in the pipeline
	if (!shader_ || &shader_->get() != &pipeline.GetShader())
	{
		shader_.reset();
		shader_.emplace(pipeline.GetShader());
		pushDescriptor_ = pipeline.IsPushDescriptor();
		descriptor_.clear();
		writeDescriptorSets_.clear();

		if (!pushDescriptor_) descriptorSet_.emplace(pipeline);

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
			auto writeDescriptorSet   = descriptor.second.writeDescriptor.GetWriteDescriptorSet();
			writeDescriptorSet.dstSet = VK_NULL_HANDLE;

			if (!pushDescriptor_)
				if (descriptorSet_) writeDescriptorSet.dstSet = *descriptorSet_;

			writeDescriptorSets_.emplace_back(writeDescriptorSet);
		}

		if (!pushDescriptor_) descriptorSet_->Update(writeDescriptorSets_);

		changed_ = false;
	}

	return true;
}

void DescriptorHandle::BindDescriptor(
	const CommandBuffer& commandBuffer, const Pipeline& pipeline) const
{
	if (pushDescriptor_)
	{
		const VkResources* vkObj = VkResources::Inst;
		Instance::CmdPushDescriptorSetKhr(vkObj->device,
			commandBuffer,
			pipeline.GetPipelineBindPoint(),
			pipeline.GetPipelineLayout(),
			0,
			static_cast<std::uint32_t>(writeDescriptorSets_.size()),
			writeDescriptorSets_.data());
	}
	else
	{
		descriptorSet_->BindDescriptor(commandBuffer);
	}
}
}    // namespace neko::vk
