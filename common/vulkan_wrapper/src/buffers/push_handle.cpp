#include "vk/buffers/push_handle.h"

namespace neko::vk
{

PushHandle::PushHandle(const UniformBlock& uniformBlock)
        : uniformBlock_(uniformBlock),
          data_(std::vector<char>(uniformBlock.GetSize())) {}

bool PushHandle::Update(const UniformBlock& uniformBlock)
{
    if (!uniformBlock_ || uniformBlock_ != uniformBlock)
    {
        uniformBlock_ = std::experimental::optional<const UniformBlock&>(uniformBlock);
        data_ = std::vector<char>(uniformBlock.GetSize());
        return false;
    }

    return true;
}

void PushHandle::BindPush(const CommandBuffer& commandBuffer, const Pipeline& pipeline) const
{
    vkCmdPushConstants(VkCommandBuffer(commandBuffer),
            pipeline.GetPipelineLayout(),
            uniformBlock_->GetStageFlags(), 0,
            static_cast<std::uint32_t>(uniformBlock_->GetSize()), &data_[0]);
}
}