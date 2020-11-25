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

#include <mathematics/vector.h>
#include "vk/vulkan_include.h"
#include "sdl_engine/sdl_window.h"
#include "graphics/graphics.h"

namespace neko::sdl
{
class VkOnResizeRenderCommand final : public RenderCommandInterface
{
public:
	void Render() override;
    void SetWindowSize(const Vec2u newWindowSize) { newWindowSize_ = newWindowSize; }
protected:
    Vec2u newWindowSize_;
};

class VulkanWindow final : public SdlWindow
{
public:
    enum WindowFlag : uint8_t
    {
        ImGuiFirstFrame = 1u << 0u,
        NEED_RESIZE = 1u << 1u,
        IS_MINIMIZED = 1u << 2u
    };

    void Init() override;

    void InitImGui() override;

    void Destroy() override;
    void GenerateUiFrame() override;
    void SwapBuffer() override;

    void OnResize(Vec2u newWindowSize) override;

    void MakeCurrentContext() override;
    void LeaveCurrentContext() override;
protected:
    void RenderUi() override;

    SDL_Event event_{};

    WindowFlag state_{};
    VkOnResizeRenderCommand onResizeCommand_;
};
}
#endif