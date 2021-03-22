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
#ifdef NEKO_GLES3
#include "gl/graphics.h"
#include "gl/gles3_window.h"
#elif NEKO_VULKAN
#include "vk/graphics.h"
#include "vk/renderers/renderer_editor.h"
#endif

#include "aer/aer_engine.h"


int main(int, char**)
{
	neko::Configuration config;
	config.windowName = "AerEditor Version 0.01";
	config.flags = neko::Configuration::NONE;
	config.windowSize = neko::Vec2u(1280, 720);

	neko::Filesystem filesystem;
	neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::GAME);
#ifdef NEKO_GLES3
	neko::sdl::Gles3Window window;
	neko::gl::Gles3Renderer renderer;
#elif NEKO_VULKAN
	neko::sdl::VulkanWindow window;
	neko::vk::VkRenderer renderer(&window);
	renderer.SetRenderer(std::make_unique<neko::vk::RendererEditor>());
#endif

	engine.SetWindowAndRenderer(&window, &renderer);

	engine.Init();
	engine.EngineLoop();
	return 0;
}
