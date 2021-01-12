#include "vk/buffers/uniform_handle.h"

namespace neko::vk
{
UniformHandle::UniformHandle(const bool multiPipeline) : multiPipeline_(multiPipeline)
{}

UniformHandle::UniformHandle(const UniformBlock& uniformBlock, bool multiPipeline)
    : multiPipeline_(multiPipeline),
    size_(uniformBlock.GetSize()),
    arbitraryUniformData_(std::vector<char>(size_)),
    uniformBlock_(uniformBlock),
    uniformBuffer_(std::make_unique<UniformBuffer>(
                    static_cast<VkDeviceSize>(size_)))
{}

void UniformHandle::Destroy() const
{
	if (uniformBuffer_) uniformBuffer_->Destroy();
}

UniformHandle::UniformHandle(const UniformHandle& other)
        : multiPipeline_(other.multiPipeline_),
          size_(other.size_),
          arbitraryUniformData_(other.arbitraryUniformData_),
          uniformBlock_(other.uniformBlock_),
          handleStatus_(other.handleStatus_)
{
    if (other.uniformBuffer_)
        uniformBuffer_ = std::make_unique<UniformBuffer>(*other.uniformBuffer_);
}

UniformHandle::UniformHandle(UniformHandle&& other) noexcept
        : multiPipeline_(other.multiPipeline_),
          size_(other.size_),
          arbitraryUniformData_(std::move(other.arbitraryUniformData_)),
          uniformBlock_(other.uniformBlock_),
          uniformBuffer_(std::move(other.uniformBuffer_)),
          handleStatus_(other.handleStatus_) {}

UniformHandle& UniformHandle::operator=(const UniformHandle& other)
{
    if (this == &other)
        return *this;

    multiPipeline_ = other.multiPipeline_;
    uniformBlock_ = other.uniformBlock_;
    size_ = other.size_;
    arbitraryUniformData_ = other.arbitraryUniformData_;
    if (uniformBuffer_)
        uniformBuffer_ = std::make_unique<UniformBuffer>(*other.uniformBuffer_);

    handleStatus_ = other.handleStatus_;
    return *this;
}

UniformHandle& UniformHandle::operator=(UniformHandle&& other) noexcept
{
    if (this == &other)
        return *this;

    multiPipeline_ = other.multiPipeline_;
    uniformBlock_ = other.uniformBlock_;
    size_ = other.size_;
    arbitraryUniformData_ = std::move(other.arbitraryUniformData_);
    uniformBuffer_ = std::move(other.uniformBuffer_);
    handleStatus_ = other.handleStatus_;
    return *this;
}

void UniformHandle::PushUniformData(const Material::PushDataContainer& dataContainer)
{
    for (const auto& data : dataContainer)
    {
	    switch (data.second.GetType())
	    {
	    	case MaterialExportData::Type::FLOAT:
			    Push(data.first, data.second.GetFloat());
		    case MaterialExportData::Type::INT:
			    Push(data.first, data.second.GetInt());
		    case MaterialExportData::Type::IMAGE_2D:
			    Push(data.first, data.second.GetImage2d());
			/*case MaterialExportData::Type::IMAGE_CUBE:
			    Push(data.first, data.second.GetImageCube());*/
		    case MaterialExportData::Type::COLOR:
			    Push(data.first, data.second.GetColor());
		    default:
			    Push(data.first, nullptr);
	    }
    }
}

bool UniformHandle::Update(const UniformBlock& uniformBlock)
{
    if (handleStatus_ == Buffer::Status::RESET ||
        (multiPipeline_ && !uniformBlock_) ||
        (!multiPipeline_ && (!uniformBlock_ || uniformBlock_ != uniformBlock)))
    {
        if ((size_ == 0 && !uniformBlock_) ||
            (uniformBlock_ && uniformBlock_ != uniformBlock &&
             uniformBlock_->GetSize() == size_))
        {
            size_ = static_cast<std::uint32_t>(uniformBlock.GetSize());
        }

        uniformBlock_ = std::neko::optional<const UniformBlock&>(uniformBlock);
        arbitraryUniformData_ = std::vector<char>(size_);

        uniformBuffer_ = std::make_unique<UniformBuffer>(static_cast<VkDeviceSize>(size_));
        handleStatus_ = Buffer::Status::CHANGED;

        return false;
    }

    if (handleStatus_ != Buffer::Status::NORMAL)
    {
        uniformBuffer_->Update(arbitraryUniformData_);
        handleStatus_ = Buffer::Status::NORMAL;
    }

    return true;
}
}
