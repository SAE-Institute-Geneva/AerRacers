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

#include "showroom/texture.h"

#include <fmt/format.h>
#include "stb_image.h"

#include <engine/log.h>

#include "utils/file_utility.h"

namespace neko::sr
{
/*void TextureManager::CreateTexture()
{
	const auto textureId = currentUploadedTexture_.textureId;
	const auto flags = currentUploadedTexture_.flags;
	auto& image = currentUploadedTexture_.image;
	if (image.data == nullptr)
	{
		textureMap_[textureId] = {};
		return;
	}

	TextureName texture;
	glCheckError();
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, flags & neko::Texture::CLAMP_WRAP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, flags & neko::Texture::CLAMP_WRAP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, flags & neko::Texture::SMOOTH_TEXTURE ? GL_LINEAR : GL_NEAREST);
	glCheckError();
	if (flags & neko::Texture::MIPMAPS_TEXTURE)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		                flags & neko::Texture::SMOOTH_TEXTURE ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR);
		glCheckError();
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, flags & neko::Texture::SMOOTH_TEXTURE ? GL_LINEAR : GL_NEAREST);
		glCheckError();
	}

	GLenum internalFormat = 0;
	GLenum dataFormat = 0;
	if (flags & neko::Texture::HDR)
	{
		switch (image.nbChannels)
		{
			case 1:
				internalFormat = GL_R16F;
				dataFormat = GL_RED;
				break;
			case 2:
				internalFormat = GL_RG16F;
				dataFormat = GL_RG;
				break;
			case 3:
				internalFormat = GL_RGB16F;
				dataFormat = GL_RGB;
				break;
			case 4:
				internalFormat = GL_RGBA16F;
				dataFormat = GL_RGBA;
				break;
			default:
				break;
		}
	}
	else
	{
		switch (image.nbChannels)
		{
			case 1:
			{
				internalFormat = GL_R8;
				dataFormat = GL_RED;
				break;
			}
			case 2:
			{
				internalFormat = GL_RG8;
				dataFormat = GL_RG;
				break;
			}
			case 3:
			{
				internalFormat = flags & neko::Texture::GAMMA_CORRECTION ? GL_SRGB8 : GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case 4:
			{
				internalFormat = flags & neko::Texture::GAMMA_CORRECTION ? GL_SRGB8_ALPHA8 : GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			default:
				break;
		}
	}

	if (!(flags & neko::Texture::HDR))
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, dataFormat, GL_UNSIGNED_BYTE, image.data);
		glCheckError();
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, dataFormat, GL_FLOAT, (float*)image.data);
		glCheckError();
	}

	if (flags & neko::Texture::MIPMAPS_TEXTURE)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		glCheckError();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	textureMap_[textureId] = {texture, {currentUploadedTexture_.image.width, currentUploadedTexture_.image.height}};
}

void TextureManager::Destroy()
{
	for(auto& textureName : textureMap_)
	{
		DestroyTexture(textureName.second.name);
		textureName.second.name = INVALID_TEXTURE_NAME;
	}

	neko::TextureManager::Destroy();
}

TextureId TextureManager::LoadTexture(std::string_view path, neko::Texture::TextureFlags flags)
{
	if (!FileExists(path))
	{
		//Texture is already in queue or even loaded
		logDebug("[Texture Manager] Texture does not exist");
		return INVALID_TEXTURE_ID;
	}

	TextureId textureId;
	textureId.ab = HashString(path.substr(0, path.size() / 2));
	textureId.cd = HashString(path.substr(path.size() / 2, path.size()));

	const auto it = texturePathMap_.find(textureId);
	if (it != texturePathMap_.end())
	{
		//Texture is already in queue or even loaded
		logDebug("[Texture Manager] Texture is already loaded");
		return textureId;
	}

	logDebug(fmt::format("[Texture Manager] Loading texture path: {}", path));
	texturePathMap_[textureId] = std::string(path.data());

#ifndef NEKO_SAMETHREAD
	//Put texture in queue
	TextureInfo textureInfo;
	textureInfo.textureId = textureId;
	textureInfo.flags = flags;
	texturesToLoad_.push(std::move(textureInfo));
#else
	textureLoader_.Reset();

    textureLoader_.SetTextureId(textureId);
    textureLoader_.SetTextureFlags(flags);
    textureLoader_.LoadFromDisk();

    auto& textureInfo = texturesToUpload_.front();
    currentUploadedTexture_ = std::move(textureInfo);
    texturesToUpload_.pop();
    uploadToGpuJob_.Reset();
    uploadToGpuJob_.Execute();
#endif

	return textureId;
}


TextureName stbCreateTexture(const std::string_view filename, neko::Texture::TextureFlags flags)
{
	const std::string extension = GetFilenameExtension(filename);
	if (!FileExists(filename))
	{
		logDebug(fmt::format("[Error] Texture: {} does not exist", filename));
		return 0;
	}

	BufferFile textureFile;
	textureFile.Load(filename);

	Image image = StbImageConvert(textureFile);
	textureFile.Destroy();
	if (image.data == nullptr)
	{
		logDebug(fmt::format("[Error] Texture: cannot load {}", filename));
		return INVALID_TEXTURE_NAME;
	}

	TextureName texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, flags & neko::Texture::CLAMP_WRAP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, flags & neko::Texture::CLAMP_WRAP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, flags & neko::Texture::SMOOTH_TEXTURE ? GL_LINEAR : GL_NEAREST);
	if (flags & neko::Texture::MIPMAPS_TEXTURE)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		                flags & neko::Texture::SMOOTH_TEXTURE ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, flags & neko::Texture::SMOOTH_TEXTURE ? GL_LINEAR : GL_NEAREST);
	}

	if (extension == ".jpg" || extension == ".tga")
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
	}
	else if (extension == ".png")
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	}
	else if (extension == ".hdr")
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, image.width, image.height, 0, GL_RGB, GL_FLOAT, image.data);
	}

	if (flags & neko::Texture::MIPMAPS_TEXTURE)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	image.Destroy();
	return texture;
}

TextureName LoadCubemap(std::vector<std::string> facesFilename)
{
	TextureName textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < facesFilename.size(); i++)
	{
		BufferFile textureFile;
		textureFile.Load(facesFilename[i]);
		const auto extension = GetFilenameExtension(facesFilename[i]);

		Image image = StbImageConvert(textureFile);
		textureFile.Destroy();
		if (image.data != nullptr)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			             0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data
			);
		}
		else
		{
			logDebug(fmt::format("[Error] Cubemap tex failed to load at path: {}", facesFilename[i]));
		}
		image.Destroy();
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glCheckError();
	return textureID;
}

void DestroyTexture(TextureName textureName)
{
	glDeleteTextures(1, &textureName);
	textureName = INVALID_TEXTURE_NAME;
}*/

Image::~Image() { Destroy(); }

Image::Image(Image&& image) noexcept
   : data(image.data), width(image.width), height(image.height), nbChannels(image.nbChannels)
{
	image.data = nullptr;
}

Image& Image::operator=(Image&& image) noexcept
{
	data       = image.data;
	image.data = nullptr;
	width      = image.width;
	height     = image.height;
	nbChannels = image.nbChannels;
	return *this;
}

void Image::Destroy()
{
	if (data) { stbi_image_free(data); }
	data   = nullptr;
	height = -1;
	width  = -1;
}

void Image::SetFormat(const std::string_view& extension)
{
	if (extension == ".png" && nbChannels == 4)
	{
		internalFormat = GL_SRGB8_ALPHA8;
		format         = GL_RGBA;
		type           = GL_UNSIGNED_BYTE;
	}
	else if ((extension == ".jpg" || extension == ".tga" || extension == ".png") && nbChannels == 3)
	{
		internalFormat = GL_SRGB;
		format         = GL_RGB;
		type           = GL_UNSIGNED_BYTE;
	}
	else if (extension == ".hdr" && nbChannels == 3)
	{
		internalFormat = GL_RGB16F;
		format         = GL_RGB16F;
		type           = GL_FLOAT;
	}
	else if (nbChannels == 1)
	{
		internalFormat = GL_RED;
		format         = GL_RED;
		type           = GL_UNSIGNED_BYTE;
	}
	else
	{
		internalFormat = GL_INVALID_OPERATION;
		format         = GL_INVALID_OPERATION;
		type           = GL_INVALID_OPERATION;
	}
}

Image StbImageConvert(const BufferFile& imageFile, bool flipY, bool hdr)
{
	Image image;
	stbi_set_flip_vertically_on_load(flipY);
	if (hdr)
	{
		image.data =
			(unsigned char*) stbi_loadf_from_memory((unsigned char*) (imageFile.dataBuffer),
				imageFile.dataLength,
				&image.width,
				&image.height,
				&image.nbChannels,
				0);
	}
	else
	{
		image.data = stbi_load_from_memory((unsigned char*) (imageFile.dataBuffer),
			imageFile.dataLength,
			&image.width,
			&image.height,
			&image.nbChannels,
			0);
	}

	return image;
}

TextureName StbCreateTexture(const std::string_view filename,
	const FilesystemInterface& filesystem,
	Texture::TextureFlags flags)
{
	const std::string extension = GetFilenameExtension(filename);
	if (!filesystem.FileExists(filename))
	{
		logDebug(fmt::format("[Error] Texture: {} does not exist", filename));
		return INVALID_TEXTURE_NAME;
	}

	BufferFile textureFile = filesystem.LoadFile(filename);
	Image image = sr::StbImageConvert(textureFile, flags & Texture::TextureFlags::FLIP_Y);
	image.SetFormat(extension);
	textureFile.Destroy();

	if (!image.data)
	{
		logDebug(fmt::format("[Error] Texture: cannot load {}", filename));
		return INVALID_TEXTURE_NAME;
	}

	TextureName texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,
	                GL_TEXTURE_WRAP_S,
	                flags & Texture::CLAMP_WRAP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,
	                GL_TEXTURE_WRAP_T,
	                flags & Texture::CLAMP_WRAP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,
	                GL_TEXTURE_MAG_FILTER,
	                flags & Texture::SMOOTH_TEXTURE ? GL_LINEAR : GL_NEAREST);

	if (flags & Texture::MIPMAPS_TEXTURE)
	{
		glTexParameteri(GL_TEXTURE_2D,
		                GL_TEXTURE_MIN_FILTER,
		                flags & Texture::SMOOTH_TEXTURE ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,
		                GL_TEXTURE_MAG_FILTER,
		                flags & Texture::SMOOTH_TEXTURE ? GL_LINEAR : GL_NEAREST);
	}

	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             image.internalFormat,
	             image.width,
	             image.height,
	             0,
	             image.format,
	             image.type,
	             image.data);

	if (flags & Texture::MIPMAPS_TEXTURE) { glGenerateMipmap(GL_TEXTURE_2D); }
	image.Destroy();

	return texture;
}

TextureName LoadCubemap(
	const std::vector<std::string>& facesFilename, const FilesystemInterface& filesystem)
{
	TextureName textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < facesFilename.size(); i++)
	{
		BufferFile textureFile = filesystem.LoadFile(facesFilename[i]);
		const auto extension   = GetFilenameExtension(facesFilename[i]);
		Image image = sr::StbImageConvert(textureFile);
		image.SetFormat(extension);
		textureFile.Destroy();

		if (image.data != nullptr)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				image.internalFormat,
				image.width,
				image.height,
				0,
				image.format,
				image.type,
				image.data);
		}
		else
		{
			logDebug(
				fmt::format("[Error] Cubemap tex failed to load at path: {}", facesFilename[i]));
		}
		image.Destroy();
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glCheckError();
	return textureID;
}

void DestroyTexture(TextureName textureName)
{
	glDeleteTextures(1, &textureName);
	textureName = INVALID_TEXTURE_NAME;
}

TextureLoader::TextureLoader(
	std::string_view path, TextureId textureId, Texture::TextureFlags flags)
   : filesystem_(BasicEngine::GetInstance()->GetFilesystem()),
	 textureId_(textureId),
	 path_(path),
	 flags_(flags),
	 loadingTextureJob_([this]() { LoadTexture(); }),
	 decompressTextureJob_([this]() { DecompressTexture(); }),
	 uploadToGLJob_([this]() { UploadToGL(); })
{}

TextureLoader::TextureLoader(TextureLoader&& textureLoader) noexcept
   : filesystem_(BasicEngine::GetInstance()->GetFilesystem()),
	 textureId_(textureLoader.textureId_),
	 path_(textureLoader.path_),
	 flags_(textureLoader.flags_),
	 loadingTextureJob_([this]() { LoadTexture(); }),
	 decompressTextureJob_([this]() { DecompressTexture(); }),
	 uploadToGLJob_([this]() { UploadToGL(); })
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
	const auto& filesystem      = BasicEngine::GetInstance()->GetFilesystem();
	const std::string extension = GetFilenameExtension(path_);
	if (!filesystem.FileExists(path_))
	{
		logDebug(fmt::format("[Error] Texture: {} does not exist", path_));
		texture_.name = INVALID_TEXTURE_NAME;
		return;
	}

	BufferFile textureFile = filesystem.LoadFile(path_);
	image_ = sr::StbImageConvert(textureFile, flags_ & Texture::TextureFlags::FLIP_Y);
	image_.SetFormat(extension);
	textureFile.Destroy();

	if (image_.data == nullptr)
	{
		logDebug(fmt::format("[Error] Texture: cannot load {}", path_));
		texture_.name = INVALID_TEXTURE_NAME;
		return;
	}

	RendererLocator::get().AddPreRenderJob(&uploadToGLJob_);
}

void TextureLoader::UploadToGL()
{
	TextureName texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		flags_ & Texture::CLAMP_WRAP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		flags_ & Texture::CLAMP_WRAP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		flags_ & Texture::SMOOTH_TEXTURE ? GL_LINEAR : GL_NEAREST);

	if (flags_ & Texture::MIPMAPS_TEXTURE)
	{
		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			flags_ & Texture::SMOOTH_TEXTURE ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			flags_ & Texture::SMOOTH_TEXTURE ? GL_LINEAR : GL_NEAREST);
	}

	glTexImage2D(GL_TEXTURE_2D,
		0,
		image_.internalFormat,
		image_.width,
		image_.height,
		0,
		image_.format,
		image_.type,
		image_.data);
	glCheckError();

	if (flags_ & Texture::MIPMAPS_TEXTURE) { glGenerateMipmap(GL_TEXTURE_2D); }
	image_.Destroy();

	texture_.name = texture;
}

TextureManager::TextureManager() : filesystem_(BasicEngine::GetInstance()->GetFilesystem())
{
	TextureManagerLocator::provide(this);
}

void TextureManager::Init() {}

void TextureManager::Update(seconds)
{
	while (!textureLoaders_.empty())
	{
		auto& textureLoader = textureLoaders_.front();
		if (textureLoader.HasErrors())
		{
			switch (textureLoader.GetErrors())
			{
				case TextureLoader::TextureLoaderError::ASSET_LOADING_ERROR:
					logDebug(fmt::format(
						"[Error] Could not load texture {} from disk", textureLoader.GetPath()));
					break;
				case TextureLoader::TextureLoaderError::DECOMPRESS_ERROR:
					logDebug(fmt::format("[Error] Could not decompress texture {} from disk",
						textureLoader.GetPath()));
					break;
				case TextureLoader::TextureLoaderError::UPLOAD_TO_GPU_ERROR:
					logDebug(fmt::format(
						"[Error] Could not upload texture {} from disk", textureLoader.GetPath()));
					break;
				default: break;
			}

			textureLoaders_.pop();
		}
		else if (textureLoader.IsDone())
		{
			textureMap_[textureLoader.GetTextureId()] = textureLoader.GetTexture();
			textureLoaders_.pop();
		}
		else
		{
			break;
		}
	}
}

void TextureManager::Destroy()
{
	for (auto& textureName : textureMap_)
	{
		DestroyTexture(textureName.second.name);
		textureName.second.name = INVALID_TEXTURE_NAME;
	}
}

const Texture* TextureManager::GetTexture(TextureId index) const
{
	const auto it = textureMap_.find(index);
	if (it != textureMap_.end()) { return &it->second; }

	return nullptr;
}

TextureName TextureManager::GetTextureName(TextureId textureId) const
{
	const auto* texture = GetTexture(textureId);
	if (texture == nullptr) { return INVALID_TEXTURE_NAME; }

	return texture->name;
}

bool TextureManager::IsTextureLoaded(TextureId textureId) const
{
	const auto it = textureMap_.find(textureId);
	return it != textureMap_.end();
}

TextureId TextureManager::LoadTexture(std::string_view path, neko::Texture::TextureFlags flags)
{
	const auto it = texturePathMap_.find(path.data());
	if (it != texturePathMap_.end()) { return it->second; }

#ifdef linux
	std::string sPath = path.data();
	sPath.replace(path.find('\\'), 1, "/");
	path = sPath;
#endif

	if (!FileExists(path))
	{
		//Texture is already in queue or even loaded
		logDebug("[Texture Manager] Texture does not exist");
		return INVALID_TEXTURE_ID;
	}

	TextureId textureId;
	textureId.ab = HashString(path.substr(0, path.size() / 2));
	textureId.cd = HashString(path.substr(path.size() / 2, path.size()));

	logDebug(fmt::format("[Texture Manager] Loading texture path: {}", path));

	textureLoaders_.push(TextureLoader(path, textureId, Texture::TextureFlags(flags)));
	textureLoaders_.back().Start();
	texturePathMap_[path.data()] = textureId;

	return textureId;
}
}    // namespace neko::sr
