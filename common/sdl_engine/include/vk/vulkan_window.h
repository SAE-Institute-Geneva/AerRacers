#pragma once
/*
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
 */

#ifdef NEKO_VULKAN
#include "sdl_engine/sdl_window.h"

namespace neko::sdl
{
class VulkanWindow final : public SdlWindow
{
public:
	[[nodiscard]] SDL_Window* GetWindow() { return window_; }

	void Init() override;
	void Destroy() override;

	/// Loop to stop the window from being rendered while it is minimized
	void MinimizedLoop() const;

protected:
	/// Unused: Required by SdlWindow
	void InitImGui() override {}
	void RenderUi() override {}
	void GenerateUiFrame() override {}
	void OnResize(Vec2u) override {}
	void SwapBuffer() override {}
	/// Unused: Required by SdlWindow
};
}    // namespace neko::sdl
#endif