#include "vk/vk_resources.h"

namespace neko::vk
{
TextureLoader::TextureLoader(
	std::string_view path, StringHash textureId, Texture::TextureFlags flags)
   : filesystem_(BasicEngine::GetInstance()->GetFilesystem()),
	 flags_(flags),
	 loadingTextureJob_([this]() { LoadTexture(); }),
	 decompressTextureJob_([this]() { DecompressTexture(); }),
	 uploadJob_([this]() { Upload(); }),
	 path_(path),
	 textureId_(textureId)
{}

TextureLoader::TextureLoader(TextureLoader&& other) noexcept
   : filesystem_(other.filesystem_),
	 flags_(other.flags_),
	 vdi_(other.vdi_),
	 loadingTextureJob_([this]() { LoadTexture(); }),
	 decompressTextureJob_([this]() { DecompressTexture(); }),
	 uploadJob_([this]() { Upload(); }),
	 path_(other.path_),
	 texture_(other.texture_),
	 textureId_(other.textureId_)
{}

void TextureLoader::Start()
{
	BasicEngine::GetInstance()->ScheduleJob(&loadingTextureJob_, JobThreadType::RESOURCE_THREAD);
}

void TextureLoader::LoadTexture()
{
	bufferFile_ = filesystem_.get().LoadFile(path_);
	if (!bufferFile_.dataBuffer)
	{
		error_ = TextureLoaderError::ASSET_LOADING_ERROR;
		return;
	}

	BasicEngine::GetInstance()->ScheduleJob(&decompressTextureJob_, JobThreadType::RESOURCE_THREAD);
}

void TextureLoader::DecompressTexture()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Create KTX from memory");
#endif
	KTX_error_code result;
	{
		result = ktxTexture_CreateFromMemory(
			reinterpret_cast<const ktx_uint8_t*>(bufferFile_.dataBuffer),
			bufferFile_.dataLength,
			KTX_TEXTURE_CREATE_NO_FLAGS,
			&kTexture_);
	}

	if (result != KTX_SUCCESS)
	{
		ktxCheckError(result);
		error_ = TextureLoaderError::DECOMPRESS_ERROR;
		return;
	}

	BasicEngine::GetInstance()->ScheduleJob(&uploadJob_, JobThreadType::RESOURCE_THREAD);
}

void TextureLoader::Upload()
{
	ktxVulkanTexture texture;
	ktxVulkanDeviceInfo vdi;
	VkResources* vkObj = VkResources::Inst;
	ktxVulkanDeviceInfo_Construct(&vdi,
		vkObj->gpu,
		vkObj->device,
		vkObj->device.GetGraphicsQueue(),
		vkObj->GetCurrentCmdPool(),
		nullptr);
	const auto res = ktxTexture_VkUploadEx(kTexture_, &vdi, &texture, kTiling, kUsage, kLayout);
	ktxTexture_Destroy(kTexture_);
	ktxVulkanDeviceInfo_Destruct(&vdi);
	if (res != KTX_SUCCESS)
	{
		error_ = TextureLoaderError::UPLOAD_TO_GPU_ERROR;
		ktxCheckError(res);
		return;
	}

	VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	if (flags_ & Texture::REPEAT_WRAP) addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	else if (flags_ & Texture::MIRROR_REPEAT_WRAP)
		addressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

	texture_.CreateFromKtx(texture,
		flags_ & Texture::SMOOTH_TEXTURE ? VK_FILTER_LINEAR : VK_FILTER_NEAREST,
		addressMode,
		false);
}
}    // namespace neko::vk