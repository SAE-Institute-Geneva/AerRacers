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

#include "gl/texture.h"
#include "gl/shape.h"

namespace neko::aer
{
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
class UiElement
{
public : 
	enum UiFlags : uint8_t
	{
		INIT = 1u << 0u,
		DIRTY = 1u << 1u
	};

    explicit UiElement(const Vec3f& pos = Vec3f::zero,
        const Vec2u& size               = Vec2u::one,
        UiAnchor uiAnchor               = UiAnchor::CENTER);
	virtual void Destroy();

	void SetPosition(const Vec3f& pos) { position_ = pos; }
	void SetSize(const Vec2u& size) { size_ = size; }
	void SetAnchor(UiAnchor uiAnchor) { uiAnchor_ = uiAnchor; }

	uint8_t flags = 0;
protected :
    Vec2f CalculateUiElementPosition(Vec2f position, Vec2f windowSize, UiAnchor anchor);
	Vec3f position_ = Vec3f::zero; //In percent
	Vec2u size_ = Vec2u(100u); //In pixel
	UiAnchor uiAnchor_ = UiAnchor::CENTER;


};
}