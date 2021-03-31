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
#include "graphics/font.h"

#include "aer/ui/ui_element.h"

#ifdef NEKO_GLES3
#include "gl/font.h"
#endif

namespace neko::aer
{
/**
 * \brief Enum of loaded font
 */
enum class FontLoaded : std::uint8_t
{
	LOBSTER,
	ROBOTO
};

/**
 * \brief  UiText use to display text on screen
 */
class UiText : public UiElement
{
public:
    /**
     * \brief Create an UiText
     * \param font Font to use
     * \param text Text to display
     * \param position Position on view scale (bottom_left:-1,-1; top_right:1,1)
     * \param anchor Anchor from screen corner
     * \param screenId On which screen are based the anchor
     * \param scale Scale of the text
     * \param color Color of the text
     */
	UiText(FontLoaded font    = FontLoaded::LOBSTER,
		std::string_view text = "",
		Vec2i position        = Vec2i::zero,
		UiAnchor anchor       = UiAnchor::CENTER,
		std::uint8_t screenId = 0,
		float scale           = 1.0f,
		const Color4& color   = Color::white)
	   : UiElement(position, anchor, screenId),
		 font_(font),
		 text_(text),
		 scale_(scale),
		 color_(color)
	{}

#ifdef NEKO_GLES3
	void Draw(gl::FontManager& fontManager, const FontId& fontId) const;
#else
#endif

	void Destroy() override;

	FontLoaded GetFont() const { return font_; }

	void SetFont(const FontLoaded& font) { font_ = font; }
	void SetText(const std::string& text) { text_ = text; }
	void SetScale(const float& scale) { scale_ = scale; }
	void SetColor(const Color4& color) { color_ = color; }

protected:
	FontLoaded font_ {};
	std::string text_ {};
	float scale_ {};
	Color4 color_ {};

	Job preRender_;
};
}    // namespace neko::aer
