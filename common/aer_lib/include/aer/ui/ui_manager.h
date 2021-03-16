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
#include "ui_text.h"
#include "graphics/graphics.h"
#include "aer/ui/ui_image.h"
#include "gl/font.h"
#include "gl/shader.h"
#include "sdl_engine/sdl_engine.h"

namespace neko::aer
{
class AerEngine;
const static size_t kMaxUiElements = 16;

enum class FontLoaded {
    LOBSTER,
    ROBOTO
};

//-----------------------------------------------------------------------------
// UiManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class IUiManager
{
public:
	virtual void RenderUiImage(UiImage* image) = 0;
	virtual void RenderUiText(FontLoaded fontPath,
        const std::string_view& text,
        const Vec2f& position = Vec2f::zero,
        UiAnchor anchor = UiAnchor::CENTER,
        float scale = 1.0f,
        const Color4& color = Color::white) = 0;
protected:
	~IUiManager() = default;
};

//-----------------------------------------------------------------------------
// NullUiManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullUiManager final : public IUiManager
{
public:
    void RenderUiImage(UiImage*) override {}
    void RenderUiText(FontLoaded,
        const std::string_view&,
        const Vec2f&,
        UiAnchor,
        float,
        const Color4&) override {}
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

	void Render() override;

	void OnEvent(const SDL_Event& event) override;

	void Destroy() override;

    void RenderUiImage(UiImage*) override;
    
    void RenderUiText(FontLoaded fontPath,
        const std::string_view& text,
        const Vec2f& position = Vec2f::zero,
        UiAnchor anchor = UiAnchor::CENTER,
        float scale = 1.0f,
        const Color4& color = Color::white) override;
private:
    AerEngine& aerEngine_;
    Job preRender_;
    //Font 
    gl::FontManager fontManager_;
    const std::string kRobotoPath_ = "font/Lobster-Regular.ttf";
    const std::string kLobsterPath_ = "font/Lobster-Regular.ttf";
    FontId robotoId_ = INVALID_FONT_ID;
    FontId lobsterId_ = INVALID_FONT_ID;

    //UiImage
    gl::Shader uiImageShader_;
    std::vector<UiImage*> uiImages_{};
};

using UiManagerLocator = Locator<IUiManager, NullUiManager>;
}
