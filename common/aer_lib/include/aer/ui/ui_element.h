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
#include "mathematics/vector.h"

namespace neko::aer
{
/**
 * \brief Anchor from screen corner
 */
enum class UiAnchor
{
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
struct UiFlag
{
	enum Enum : std::uint8_t
	{
		ENABLED     = 1u << 0u,
		INITIALIZED = 1u << 1u,
		DIRTY       = 1u << 2u,
	};
};

/**
 * \brief Class based of UiElements
 */
class UiElement
{
public:
	explicit UiElement(const Vec3f& pos = Vec3f::zero,
		UiAnchor uiAnchor               = UiAnchor::CENTER,
		std::uint8_t screenId           = 0);
	virtual void Destroy();

	[[nodiscard]] std::uint8_t GetFlags() const { return flags_; }
	[[nodiscard]] std::uint8_t GetScreenId() const { return screenId_; }

	void SetPosition(const Vec3f& pos) { position_ = pos; }
	void SetAnchor(UiAnchor uiAnchor) { uiAnchor_ = uiAnchor; }
	void SetScreenId(std::uint8_t screenId) { screenId_ = screenId; }
	void SetEnable(bool enable);

	void AddFlag(UiFlag::Enum flag);
	void RemoveFlag(UiFlag::Enum flag);

protected:
    virtual ~UiElement() = default;
    
	static Vec2f CalculateUiElementPosition(Vec2f position, UiAnchor anchor);

	Vec3f position_    = Vec3f::zero;    //In percent
	UiAnchor uiAnchor_ = UiAnchor::CENTER;
	std::uint8_t flags_     = UiFlag::ENABLED;
	std::uint8_t screenId_  = 0;
};
}    // namespace neko::aer
