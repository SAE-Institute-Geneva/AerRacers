#include "vk/buffers/uniform_handle.h"

namespace neko::vk
{
UniformHandle::UniformHandle(const bool multiPipeline) : multiPipeline_(multiPipeline) {}

UniformHandle::UniformHandle(const UniformBlock& uniformBlock, bool multiPipeline)
   : multiPipeline_(multiPipeline),
	 size_(uniformBlock.GetSize()),
	 arbitraryUniformData_(std::vector<char>(size_)),
	 uniformBlock_(uniformBlock),
	 uniformBuffer_(size_)
{}

UniformHandle::~UniformHandle() { Destroy(); }

void UniformHandle::Destroy() const
{
	if (uniformBuffer_) uniformBuffer_->Destroy();
}

void UniformHandle::PushUniformData(const Material::PushDataContainer& dataContainer)
{
	for (const auto& data : dataContainer)
		Push(data.first, data.second);
}

bool UniformHandle::Update(const UniformBlock& uniformBlock)
{
	if (handleStatus_ == Buffer::Status::RESET || (multiPipeline_ && !uniformBlock_) ||
		(!multiPipeline_ && (!uniformBlock_ || uniformBlock_->get() != uniformBlock)))
	{
		if ((size_ == 0 && !uniformBlock_) ||
			(uniformBlock_ && uniformBlock_->get() != uniformBlock &&
				uniformBlock_->get().GetSize() == size_))
		{
			size_ = uniformBlock.GetSize();
		}

		uniformBlock_.emplace(uniformBlock);
		arbitraryUniformData_ = std::vector<char>(size_);

		uniformBuffer_.emplace(size_);
		handleStatus_  = Buffer::Status::CHANGED;

		return false;
	}

	if (handleStatus_ != Buffer::Status::NORMAL)
	{
		uniformBuffer_->Update(arbitraryUniformData_);
		handleStatus_ = Buffer::Status::NORMAL;
	}

	return true;
}
}    // namespace neko::vk
