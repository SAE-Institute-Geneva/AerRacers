#pragma once
#include "optional.hpp"

#include "vk/buffers/buffer.h"
#include "vk/shaders/uniform.h"

namespace neko::vk
{
struct UniformBlock;
struct UniformBuffer;

class UniformHandle
{
public:
    explicit UniformHandle(bool multiPipeline = false);
    explicit UniformHandle(const UniformBlock& uniformBlock, bool multiPipeline = false);
	void Destroy() const;

    UniformHandle(const UniformHandle& other);
    UniformHandle(UniformHandle&& other) noexcept;

    ~UniformHandle() = default;

    UniformHandle& operator=(const UniformHandle& other);
    UniformHandle& operator=(UniformHandle&& other) noexcept;

    //void PushUniformData(const Material::PushDataContainer& dataContainer);

    template <typename T>
    void Push(const T& object, const std::size_t offset, const std::size_t size)
    {
        if (std::memcmp(&arbitraryUniformData_[0] + offset, &object, size) != 0)
        {
            std::memcpy(&arbitraryUniformData_[0] + offset, &object, size);
            handleStatus_ = Buffer::Status::CHANGED;
        }
    }

    template <typename T>
    void Push(const XXH64_hash_t uniformHash, const T& object, const std::size_t size = 0)
    {
        if (!uniformBlock_) return;

        const auto& uniform = uniformBlock_->GetUniform(uniformHash);

        auto realSize = size;

        if (realSize == 0)
            realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform.GetSize()));

        Push(object, static_cast<std::size_t>(uniform.GetOffset()), realSize);
    }

    bool Update(const UniformBlock& uniformBlock);

    [[nodiscard]] const UniformBuffer& GetUniformBuffer() const { return *uniformBuffer_; }

private:
    bool multiPipeline_;

    std::uint32_t size_ = 0;
    std::vector<char> arbitraryUniformData_{};

    std::neko::optional<const UniformBlock&> uniformBlock_ = std::neko::nullopt;
    std::unique_ptr<UniformBuffer> uniformBuffer_ = nullptr;

    Buffer::Status handleStatus_ = Buffer::Status::NORMAL;
};
}