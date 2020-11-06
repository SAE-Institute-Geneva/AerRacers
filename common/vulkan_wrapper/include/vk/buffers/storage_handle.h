#pragma once
#include "vk/buffers/storage_buffer.h"

namespace neko::vk
{
class StorageHandle
{
public:
    explicit StorageHandle(bool multiPipeline = false);
    explicit StorageHandle(const UniformBlock& uniformBlock, bool multiPipeline = false);
	void Destroy() const;

    void Push(const void* data, std::size_t size);

    template <typename T>
    void Push(const T& object, const std::size_t offset, const std::size_t size)
    {
        if (!uniformBlock_) return;

        if (memcmp(&arbitraryStorageData_[0] + offset, &object, size) != 0)
        {
            memcpy(&arbitraryStorageData_[0] + offset, &object, size);
            handleStatus_ = Buffer::Status::CHANGED;
        }
    }

    template <typename T>
    void Push(const XXH64_hash_t& uniformHash,
            const T& object,
            const std::size_t size = 0)
    {
        if (!uniformBlock_) return;

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

    [[nodiscard]] const StorageBuffer& GetUniformBuffer() const { return *storageBuffer_; }

private:
    bool multiPipeline_;

    std::experimental::optional<const UniformBlock&> uniformBlock_ = std::experimental::nullopt;
    std::uint32_t size_ = 0;
    std::vector<char> arbitraryStorageData_;
    std::unique_ptr<StorageBuffer> storageBuffer_ = nullptr;

    Buffer::Status handleStatus_ = Buffer::Status::NORMAL;
};
}