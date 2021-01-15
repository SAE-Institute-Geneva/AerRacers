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

#include "ktx.h"
#include <fmt/format.h>

#include <engine/log.h>

#include "showroom/texture.h"
#include "utils/file_utility.h"


namespace neko::sr
{
void TextureManager::CreateTexture()
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
}
}



