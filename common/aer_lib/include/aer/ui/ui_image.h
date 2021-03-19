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
 Author : Canas Simon
 Co-Author : Floreau Luca
 Date : 13.03.2021
---------------------------------------------------------- */
#include "graphics/color.h"

#include "aer/ui/ui_element.h"

#ifdef NEKO_GLES3
#include "gl/shader.h"
#include "gl/shape.h"
#include "gl/texture.h"
#else
#endif

namespace neko::aer
{
class UiImage : public UiElement
{
public:
	explicit UiImage(std::string_view texturePath = "",
		const Vec3f& position                     = Vec3f::zero,
		const Vec2u& size                         = Vec2u::one,
		UiAnchor anchor                           = UiAnchor::CENTER,
		std::uint8_t screenId                     = 0,
		const Color4& color                       = Color::white);

#ifdef NEKO_GLES3
	void Init(gl::TextureManager& textureManager);
	void Draw(gl::TextureManager& textureManager,
		const Vec2u& screenSize,
		const gl::Shader& uiImageShader);
#else
	void Init();
	void Draw(const Vec2u& screenSize);
#endif

	void Destroy() override;

	void SetTexturePath(const std::string& texturePath) { texturePath_ = texturePath; }
	void SetSize(const Vec2u& size) { size_ = size; }
	void SetColor(const Color4& color) { color_ = color; }

protected:
	Vec2u size_ = Vec2u(100u);    //In pixel

	std::string texturePath_;
	TextureId textureId_     = INVALID_TEXTURE_ID;
	TextureName textureName_ = INVALID_TEXTURE_NAME;

#ifdef NEKO_GLES3
	gl::RenderQuad quad_ {Vec3f::zero, Vec2f::one};
#endif

	Color4 color_ = Color::white;
};
}    // namespace neko::aer
