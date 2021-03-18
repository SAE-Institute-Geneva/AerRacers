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
#include <utility>

#include "ui_element.h"
#include "gl/font.h"
#include "graphics/font.h"

namespace neko::aer
{
    enum class FontLoaded : uint8_t {
        LOBSTER,
        ROBOTO
    };
class UiText : public UiElement
{
public:
    explicit UiText(FontLoaded font = FontLoaded::LOBSTER,
        const std::string_view& text = "",
        const Vec3f& position = Vec3f::zero,
        UiAnchor anchor = UiAnchor::CENTER,
        uint8_t screenId = 0,
        float scale = 1.0f,
        const Color4& color = Color::white)
        : UiElement(position, anchor, screenId),
        font_(font),
        text_(text),
        scale_(scale),
        color_(color)
    {}

    void Draw(gl::FontManager& fontManager, const FontId& fontId) const;
	void Destroy() override;

    void SetFont(const FontLoaded& font) { font_ = font; }
    FontLoaded GetFont() const { return font_; }
    void SetText(const std::string& text) { text_ = text; }
    void SetScale(const float& scale) { scale_ = scale; }
    void SetColor(const Color4& color) { color_ = color; }

protected:
	Job preRender_;
    FontLoaded font_;
    std::string text_;
    float scale_;
    Color4 color_;

};
}
