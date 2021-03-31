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

	stbi_set_flip_vertically_on_load(flags_ & Texture::FLIP_Y);
	image_.data = stbi_load_from_memory((unsigned char*) (bufferFile_.dataBuffer),
		bufferFile_.dataLength,
		&image_.width,
		&image_.height,
		&image_.nbChannels,
		0);

	if (!image_.data)
	{
		error_ = TextureLoaderError::DECOMPRESS_ERROR;
		return;
	}

	BasicEngine::GetInstance()->ScheduleJob(&uploadJob_, JobThreadType::RESOURCE_THREAD);
}

void TextureLoader::Upload()
{
	VkFormat format;
	switch (image_.nbChannels)
	{
		case 3: format = VK_FORMAT_R8G8B8_SRGB; break;
		case 4: format = VK_FORMAT_R8G8B8A8_SRGB; break;
		default: neko_assert(false, "Unsupported channel number!");
	}

	texture_ = Image2d(path_,
		format,
		flags_ & Texture::SMOOTH_TEXTURE ? VK_FILTER_LINEAR : VK_FILTER_NEAREST,
		flags_ & Texture::REPEAT_WRAP ? VK_SAMPLER_ADDRESS_MODE_REPEAT :
                                        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		true,
		flags_ & Texture::MIPMAPS_TEXTURE,
		false);
	texture_.CreateFromStb(image_);
	image_.Destroy();
}
}    // namespace neko::vk