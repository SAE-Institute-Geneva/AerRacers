#include "vk/images/texture_loader.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
TextureLoader::TextureLoader(std::string_view path,
	ktxVulkanDeviceInfo& vdi,
	TextureId textureId,
	Texture::TextureFlags flags)
   : filesystem_(BasicEngine::GetInstance()->GetFilesystem()),
	 flags_(flags),
	 vdi_(vdi),
	 loadingTextureJob_([this]() { LoadTexture(); }),
	 decompressTextureJob_([this]() { DecompressTexture(); }),
	 uploadJob_([this]() { Upload(); }),
	 path_(path),
	 texture_(""),
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

	BasicEngine::GetInstance()->ScheduleJob(&decompressTextureJob_, JobThreadType::OTHER_THREAD);
}

void TextureLoader::DecompressTexture()
{
	KTX_error_code result;
	{
#ifdef EASY_PROFILE_USE
		EASY_BLOCK("Create KTX from memory");
#endif
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

	RendererLocator ::get().AddPreRenderJob(&uploadJob_);
}

void TextureLoader::Upload()
{
	ktxVulkanTexture texture;
	VkResources* vkObj = VkResources::Inst;

	vdi_.cmdPool   = vkObj->GetCurrentCmdPool();
	const auto res = ktxTexture_VkUploadEx(kTexture_, &vdi_, &texture, kTiling, kUsage, kLayout);
	if (res != KTX_SUCCESS)
	{
		error_ = TextureLoaderError::UPLOAD_TO_GPU_ERROR;
		ktxCheckError(res);
	}

	ktxTexture_Destroy(kTexture_);
	texture_.CreateFromKtx(texture);
}
}    // namespace neko::vk