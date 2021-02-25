#pragma once
#include "vk/images/image2d.h"

namespace neko::vk
{

class TextureLoader
{
public:
	enum class TextureLoaderError : std::uint8_t
	{
		NONE                = 0u,
		ASSET_LOADING_ERROR = 1u,
		DECOMPRESS_ERROR    = 2u,
		UPLOAD_TO_GPU_ERROR = 3u
	};

	TextureLoader(std::string_view path,
		ktxVulkanDeviceInfo& vdi,
		TextureId textureId,
		Texture::TextureFlags flags = Texture::DEFAULT);

	TextureLoader(const TextureLoader&) = delete;
	TextureLoader(TextureLoader&& other) noexcept;

	TextureLoader& operator=(const TextureLoader&) = delete;
	TextureLoader& operator=(TextureLoader&&) = default;

	void Start();

	std::string_view GetPath() const { return path_; }
	[[nodiscard]] TextureId GetTextureId() const { return textureId_; }
	[[nodiscard]] const Image2d& GetTexture() const { return texture_; }
	[[nodiscard]] TextureLoaderError GetErrors() const { return error_; }

	[[nodiscard]] bool IsDone() { return uploadJob_.IsDone(); }
	[[nodiscard]] bool HasErrors() const { return error_ != TextureLoaderError::NONE; }

private:
	void LoadTexture();
	void DecompressTexture();
	void Upload();

	std::reference_wrapper<const FilesystemInterface> filesystem_;
	Texture::TextureFlags flags_ = Texture::DEFAULT;

	ktxTexture* kTexture_        = nullptr;
	ktxVulkanDeviceInfo& vdi_;

	Job loadingTextureJob_;
	Job decompressTextureJob_;
	Job uploadJob_;

	std::string path_;
	BufferFile bufferFile_;

	Image2d texture_;
	TextureId textureId_;

	TextureLoaderError error_ = TextureLoaderError::NONE;
};
}
