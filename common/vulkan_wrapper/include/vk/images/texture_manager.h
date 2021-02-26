#pragma once
/* ----------------------------------------------------
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

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "mathematics/hash.h"

#include "vk/images/texture_loader.h"

namespace neko::vk
{
using ResourceHash = StringHash;
constexpr ResourceHash kInvalidTextureId = 0;

class ITextureManager
{
public:
	virtual ~ITextureManager() = default;

	virtual ResourceHash AddTexture(std::string_view)                        = 0;
	virtual ResourceHash AddTexture(std::string_view, Texture::TextureFlags) = 0;
	[[nodiscard]] virtual const Image2d* GetTexture(ResourceHash) const      = 0;
	[[nodiscard]] virtual const Image2d* GetTexture(std::string_view) const  = 0;

	virtual void Clear() = 0;
};

class NullTextureManager : public ITextureManager
{
public:
	ResourceHash AddTexture(std::string_view) override { return 0; }
	ResourceHash AddTexture(std::string_view, Texture::TextureFlags) override { return 0; }

	[[nodiscard]] const Image2d* GetTexture(ResourceHash) const override
	{
		neko_assert(false, "Texture Manager is Null!");
	}

	[[nodiscard]] const Image2d* GetTexture(std::string_view) const override
	{
		neko_assert(false, "Texture Manager is Null!");
	}

	void Clear() override {}
};

class TextureManager final : public ITextureManager, public SystemInterface
{
public:
	TextureManager();

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;

	ResourceHash AddTexture(std::string_view path) override;
	ResourceHash AddTexture(std::string_view path, Texture::TextureFlags flags) override;
	[[nodiscard]] const Image2d* GetTexture(ResourceHash resourceId) const override;
	[[nodiscard]] const Image2d* GetTexture(std::string_view texturePath) const override;

	void Clear() override;

private:
	std::queue<TextureLoader> textureLoaders_;
	std::map<ResourceHash, Image2d> textureMap_;
};

using TextureManagerLocator = Locator<ITextureManager, NullTextureManager>;
}    // namespace neko::vk
