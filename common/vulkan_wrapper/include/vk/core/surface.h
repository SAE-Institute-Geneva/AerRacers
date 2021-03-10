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

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "SDL.h"

#include "vk/vk_include.h"

namespace neko::vk
{
constexpr VkFormat kFormat = VK_FORMAT_B8G8R8A8_UNORM;    //TODO(@Simon) Change to sRGB later

class Surface
{
public:
	explicit Surface() = default;

	operator const VkSurfaceKHR&() const { return surface_; }

	void Init(SDL_Window& window);
	void SetFormat();
	void Destroy() const;

	[[nodiscard]] const VkSurfaceKHR& GetSurface() const { return surface_; }
	[[nodiscard]] VkSurfaceFormatKHR GetFormat() const { return format_; }

	[[nodiscard]] VkSurfaceCapabilitiesKHR GetCapabilities() const;

private:
	VkSurfaceKHR surface_ {};
	VkSurfaceFormatKHR format_ {};
};
}    // namespace neko::vk
