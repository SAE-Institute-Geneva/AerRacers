#include "vk/buffers/storage_handle.h"

namespace neko::vk
{
StorageHandle::StorageHandle(const bool multiPipeline) : multiPipeline_(multiPipeline) {}

StorageHandle::StorageHandle(const UniformBlock& uniformBlock, bool multiPipeline)
   : multiPipeline_(multiPipeline),
     uniformBlock_(uniformBlock),
     size_(static_cast<std::uint32_t>(uniformBlock_->get().GetSize())),
     storageBuffer_(static_cast<VkDeviceSize>(size_)),
	 arbitraryStorageData_(std::vector<char>(size_))
{}

void StorageHandle::Destroy() const
{
	if (storageBuffer_) storageBuffer_->Destroy();
}

void StorageHandle::Push(const void* data, VkDeviceSize size)
{
	if (size != size_)
	{
		size_         = size;
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
	if (handleStatus_ == Buffer::Status::RESET || (multiPipeline_ && !uniformBlock_) ||
		(!multiPipeline_ && uniformBlock_->get() != uniformBlock))
	{
		if ((size_ == 0 && !uniformBlock_) ||
			(uniformBlock_ && uniformBlock_->get() != uniformBlock &&
				uniformBlock_->get().GetSize() == size_))
		{
			size_ = uniformBlock.GetSize();
		}

		uniformBlock_.emplace(uniformBlock);
		arbitraryStorageData_ = std::vector<char>(size_);

		storageBuffer_.emplace(size_);

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
}    // namespace neko::vk