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

 Author : Floreau Luca
 Co-Author :
 Date : 29.09.2020
---------------------------------------------------------- */
#include "aer/editor/editor_tool_manager.h"
#include "aer/game/game_camera.h"
#include "aer/gizmos_renderer.h"
#include "aer/ui/ui_manager.h"

namespace neko::aer
{
class AerEngine;
struct ResourceManagerContainer;
struct ComponentManagerContainer;

constexpr std::uint8_t kMaxPlayerNum = 4;

class DrawSystem final : public SystemInterface,
						 public sdl::SdlEventSystemInterface,
						 public RenderCommandInterface,
						 public DrawImGuiInterface
{
public:
	explicit DrawSystem(AerEngine& engine);

	void Init() override;
	void DrawImGui() override;
	void Update(seconds dt) override;
	void Render() override;
	void Destroy() override;

	void OnEvent(const SDL_Event& event) override;

private:
	void RenderScene(std::size_t playerNum);

	GameCamera camera_;
	AerEngine& engine_;

	ResourceManagerContainer& rContainer_;
	ComponentManagerContainer& cContainer_;

#ifdef NEKO_GLES3
	std::unique_ptr<GizmoRenderer> gizmosRenderer_;
#endif
	std::unique_ptr <UiManager> uiManager_;

	std::uint8_t playerNum_ = 1;
};
}    // namespace neko::aer
