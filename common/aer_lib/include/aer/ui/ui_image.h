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
#include "vk/images/texture_manager.h"
#endif

namespace neko::aer
{
/**
 * \brief UiImage use to display sprites on screen
 */
class UiImage : public UiElement
{
public:
	/**
     * \brief Create a UiImage
     * \param texturePath path of the texture to display
     * \param position Position in pixels relative to anchor point
     * \param size Size in pixel 
     * \param anchor Anchor from screen corner
     * \param screenId On which screen are based the anchor
     * \param color Color add to the texture
     */
	explicit UiImage(std::string_view texturePath = "",
		Vec2i position                            = Vec2i::zero,
		Vec2u size                                = Vec2u::one,
		UiAnchor anchor                           = UiAnchor::CENTER,
		std::uint8_t screenId                     = 0,
		const Color4& color                       = Color::white);

#ifdef NEKO_GLES3
	void Init(gl::TextureManager& textureManager);
	void Draw(
		gl::TextureManager& textureManager, uint8_t playerNmb, const gl::Shader& uiImageShader);
#else
	void Init();
	void Draw(const Vec2u& screenSize);
#endif

	void Destroy() override;

    /**
     * \brief Change the texture of the image
     * \param texturePath TexturePath of the new texture
     */
#ifdef NEKO_GLES3
    void ChangeTexture(gl::TextureManager& textureManager, const std::string& texturePath);
#else
	void ChangeTexture(const std::string& texturePath);
#endif

	void SetSize(const Vec2u& size) { size_ = size; }
	void SetColor(const Color4& color) { color_ = color; }

    /**
	 * \brief Crop the image
	 * \param slidingCrop Percent of the image in x and y to crop
	 */
	void SetCropping(const Vec2f& slidingCrop) { slidingCrop_ = slidingCrop; }

protected:
	[[nodiscard]] Vec2i GetPosition(const std::uint8_t playerNmb, Vec2f size) const;
	[[nodiscard]] Vec2i FixAnchorPosition(Vec2i anchorPos, std::uint8_t playerNmb) const;

	Vec2u size_ = Vec2u(100u);    //In pixel

	std::string texturePath_;

#ifdef NEKO_GLES3
	TextureId textureId_     = INVALID_TEXTURE_ID;
	TextureName textureName_ = INVALID_TEXTURE_NAME;
#else
	vk::ResourceHash textureId_ = vk::INVALID_TEXTURE_ID;
#endif
};
}    // namespace neko::aer
