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
#include "graphics/graphics.h"
#include "aer/ui/ui_element.h"
#include "gl/font.h"
#include "gl/shader.h"
#include "sdl_engine/sdl_engine.h"

namespace neko::aer
{
class AerEngine;
const static size_t kMaxUiElements = 16;

//-----------------------------------------------------------------------------
// UiManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class IUiManager
{
public:
	virtual void AddUiElement(UiElement* uiElement) = 0;
protected:
	~IUiManager() = default;
};

//-----------------------------------------------------------------------------
// NullUiManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullUiManager final : public IUiManager
{
	void AddUiElement([[maybe_unused]] UiElement* uiElement) override {}
};

//-----------------------------------------------------------------------------
// UiManager
//-----------------------------------------------------------------------------
/// \brief Draw gizmos
class UiManager final : public RenderCommandInterface,
                        public SystemInterface,
                        public sdl::SdlEventSystemInterface,
                        public IUiManager
{
public:
	explicit UiManager(AerEngine& aerEngine);

	void Init() override;

	void Update(seconds dt) override;

	void AddUiElement(UiElement* uiElement) override;

	void Render() override;

	void OnEvent(const SDL_Event& event) override;

	void Destroy() override;

private:
	gl::FontManager fontManager_;
	FontId fontId_ = INVALID_FONT_ID;
	gl::Shader uiShader_;
	std::vector<UiElement*> uiElements_{};
	AerEngine& aerEngine_;
    Job preRender_;
};

using UiManagerLocator = Locator<IUiManager, NullUiManager>;
}
