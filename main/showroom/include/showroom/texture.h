#pragma once
/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#include "graphics/texture.h"

namespace neko::sr
{
struct Image
{
	Image() = default;
	~Image();
	Image(Image&& image) noexcept;

	Image& operator=(Image&& image) noexcept;
	Image(const Image&) = delete;
	Image& operator= (const Image&) = delete;

	void Destroy();

	void SetFormat(const std::string_view& extension);

	unsigned char* data = nullptr;
	int width = -1, height = -1;
	int nbChannels = 0;
	int internalFormat = GL_RGB;
	int format = GL_RGB;
	int type = GL_UNSIGNED_BYTE;
};

Image StbImageConvert(const BufferFile& imageFile, bool flipY = false, bool hdr = false);

struct Texture : neko::Texture
{
	enum class TextureType : std::uint8_t
	{
		NONE,
		DIFFUSE,
		SPECULAR,
		NORMAL,
		EMISSIVE
	};

	enum TextureFlags : unsigned
	{
		SMOOTH_TEXTURE     = 1u << 0u,
		MIPMAPS_TEXTURE    = 1u << 1u,
		CLAMP_WRAP         = 1u << 2u,
		REPEAT_WRAP        = 1u << 3u,
		MIRROR_REPEAT_WRAP = 1u << 4u,
		GAMMA_CORRECTION   = 1u << 5u,
		FLIP_Y             = 1u << 6u,
		HDR                = 1u << 7u,
		DEFAULT            = REPEAT_WRAP | SMOOTH_TEXTURE | MIPMAPS_TEXTURE,
	};

	Texture() = default;
	TextureId textureId = INVALID_TEXTURE_ID;
	std::string sName;
	TextureType type = TextureType::NONE;
};

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

	explicit TextureLoader(
		std::string_view path, TextureId, Texture::TextureFlags flags = Texture::DEFAULT);

	TextureLoader(TextureLoader&&) noexcept;
	TextureLoader(const TextureLoader&) = delete;

	TextureLoader& operator=(TextureLoader&&) = default;
	TextureLoader& operator=(const TextureLoader&) = delete;

	void Start();
	[[nodiscard]] bool IsDone() { return uploadToGLJob_.IsDone(); }

	std::string_view GetPath() const { return path_; }
	[[nodiscard]] TextureId GetTextureId() const { return textureId_; }
	[[nodiscard]] const Texture& GetTexture() const { return texture_; }
	[[nodiscard]] TextureLoaderError GetErrors() const { return error_; }

	[[nodiscard]] bool HasErrors() const { return error_ != TextureLoaderError::NONE; }

private:
	void LoadTexture();
	void DecompressTexture();
	void UploadToGL();

	std::reference_wrapper<const FilesystemInterface> filesystem_;
	TextureId textureId_;
	std::string path_;
	Texture::TextureFlags flags_ = Texture::DEFAULT;
	Job loadingTextureJob_;
	Job decompressTextureJob_;
	Job uploadToGLJob_;
	BufferFile bufferFile_;
	Texture texture_;
	Image image_;
	TextureLoaderError error_ = TextureLoaderError::NONE;
};

class TextureManager : public neko::TextureManagerInterface, public SystemInterface
{
public:
	explicit TextureManager();

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;

	const Texture* GetTexture(TextureId index) const override;
	[[nodiscard]] TextureName GetTextureName(TextureId textureId) const;
	bool IsTextureLoaded(TextureId textureId) const override;

	TextureId LoadTexture(std::string_view path, neko::Texture::TextureFlags flags) override;

private:
	const FilesystemInterface& filesystem_;
	std::map<std::string, TextureId> texturePathMap_;
	std::map<TextureId, Texture> textureMap_;
	std::queue<TextureLoader> textureLoaders_;
};

TextureName StbCreateTexture(const std::string_view filename,
	const FilesystemInterface& filesystem,
	Texture::TextureFlags flags = Texture::DEFAULT);
TextureName LoadCubemap(
	const std::vector<std::string>& facesFilename, const FilesystemInterface& filesystem);
void DestroyTexture(TextureName textureName);
}    // namespace neko::sr