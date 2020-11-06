#pragma once
#include "optional.hpp"

#include "vk/shaders/uniform_block.h"
#include "vk/commands/command_buffer.h"
#include "vk/pipelines/pipeline.h"

namespace neko::vk
{
class PushHandle
{
public:
    explicit PushHandle() = default;
    explicit PushHandle(const UniformBlock& uniformBlock);

    template <typename T>
    void Push(const T& object,
            const std::size_t offset,
            const std::size_t size)
    {
        std::memcpy(&data_[0] + offset, &object, size);
    }

    template <typename T>
    void Push(const XXH64_hash_t uniformHash,
            const T& object,
            const std::size_t size = 0)
    {
        if (!uniformBlock_) { return; }

        const auto& uniform = uniformBlock_->GetUniform(uniformHash);

        auto realSize = size;
        if (realSize == 0)
        {
            realSize = std::min(sizeof(object),
                    static_cast<std::size_t>(uniform.GetSize()));
        }

        Push(object, static_cast<std::size_t>(uniform.GetOffset()), realSize);
    }

    bool Update(const UniformBlock& uniformBlock);

    void BindPush(const CommandBuffer& commandBuffer, const Pipeline& pipeline) const;

private:
    std::experimental::optional<const UniformBlock&> uniformBlock_ = std::experimental::nullopt;
    std::vector<char> data_;
};
}