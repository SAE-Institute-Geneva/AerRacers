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
#include <cstdint>

#include <mathematics\vector.h>

namespace neko::aer
{
/**
 * \brief Anchor from screen corner
 */
enum class UiAnchor {
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM,
    BOTTOM_RIGHT
};

/**
 * \brief Flag for UiElement state
 */
struct UiFlag {
    enum Enum : uint8_t {
        ENABLE = 1u << 0u,
        INIT = 1u << 1u,
        DIRTY = 1u << 2u,
    };
};
/**
 * \brief Class based of UiElements
 */
class UiElement
{
public :
    explicit UiElement(const Vec2f& pos = Vec2f::zero,
        UiAnchor uiAnchor               = UiAnchor::CENTER,
        uint8_t screenId = 0);
	virtual void Destroy();

	void SetPosition(const Vec2f& pos) { position_ = pos; }
	void SetAnchor(UiAnchor uiAnchor) { uiAnchor_ = uiAnchor; }
    void SetScreenId(uint8_t screenId) { screenId_ = screenId; }
    void SetEnable(bool enable);
    void AddFlag(UiFlag::Enum flag);
    void RemoveFlag(UiFlag::Enum flag);
    uint8_t GetFlags() const { return flags_; }
    uint8_t GetScreenId() const { return screenId_; }

protected :
    ~UiElement() = default;
    Vec2f CalculateUiElementPosition(Vec2f position, Vec2f windowSize, UiAnchor anchor);
    Vec2f position_ = Vec2f::zero; //In percent
	UiAnchor uiAnchor_ = UiAnchor::CENTER;
	uint8_t flags_ = UiFlag::ENABLE;
    uint8_t screenId_ = 0;


};
} // namespace neko::aerS