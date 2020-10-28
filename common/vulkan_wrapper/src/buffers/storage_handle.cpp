#include "vk/buffers/storage_handle.h"

namespace neko::vk
{
StorageHandle::StorageHandle(bool multiPipeline)
        : multiPipeline_(multiPipeline) {}

StorageHandle::StorageHandle(const UniformBlock& uniformBlock, bool multiPipeline)
        : multiPipeline_(multiPipeline),
          uniformBlock_(uniformBlock),
          size_(static_cast<uint32_t>(uniformBlock_->GetSize())),
          arbitraryStorageData_(std::vector<char>(size_)),
          storageBuffer_(std::make_unique<StorageBuffer>(
                          static_cast<VkDeviceSize>(size_))) {}

void StorageHandle::Push(const void* data, std::size_t size)
{
    if (size != size_)
    {
        size_ = static_cast<uint32_t>(size);
        handleStatus_ = Buffer::Status::RESET;
        return;
    }

    if (!uniformBlock_) return;

    if (memcmp(arbitraryStorageData_.data(), data, size) != 0)
    {
        memcpy(arbitraryStorageData_.data(), data, size);
        handleStatus_ = Buffer::Status::CHANGED;
    }
}

bool StorageHandle::Update(const UniformBlock& uniformBlock)
{
    if (handleStatus_ == Buffer::Status::RESET ||
        (multiPipeline_ && !uniformBlock_) ||
        (!multiPipeline_ && *uniformBlock_ != uniformBlock))
    {
        if ((size_ == 0 && !uniformBlock_) ||
            (uniformBlock_ && *uniformBlock_ != uniformBlock &&
             static_cast<uint32_t>(uniformBlock_->GetSize()) == size_))
        {
            size_ = static_cast<uint32_t>(uniformBlock.GetSize());
        }

        uniformBlock_ = std::experimental::optional<const UniformBlock&>(uniformBlock);
        arbitraryStorageData_ = std::vector<char>(size_);

        storageBuffer_ = std::make_unique<StorageBuffer>(static_cast<VkDeviceSize>(size_));

        handleStatus_ = Buffer::Status::CHANGED;

        return false;
    }

    if (handleStatus_ != Buffer::Status::NORMAL)
    {
        storageBuffer_->Update(arbitraryStorageData_);
        handleStatus_ = Buffer::Status::NORMAL;
    }

    return true;
}
}