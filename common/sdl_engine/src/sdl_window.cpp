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

#include <sstream>

#include <utilities/service_locator.h>
#include <engine/log.h>
#include "sdl_engine/sdl_window.h"
#include "engine/engine.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif
namespace neko
{

void sdl::SdlWindow::Init()
{

#ifdef EASY_PROFILE_USE
    EASY_BLOCK("InitSdlWindow");
#endif
    auto& config = BasicEngine::GetInstance()->config;


    auto flags = SDL_WINDOW_RESIZABLE |
#ifdef NEKO_GLES3
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
#endif
        ;
#if defined(__ANDROID__)
    //config.fullscreen = true;
    config.windowSize = Vec2u(1280, 720);
    config.fullscreen = true;
#endif
                    ;
    auto windowSize = config.windowSize;
    if (config.fullscreen)
    {
        windowSize = Vec2u::zero;
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    window_ = SDL_CreateWindow(
            config.windowName.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            windowSize.x,
            windowSize.y,
            flags
    );
	if(config.fullscreen)
	{
        int windowSizeW = 0;
        int windowSizeH = 0;
        SDL_GetWindowSize(window_, &windowSizeW, &windowSizeH);
        windowSize.x = windowSizeW;
        windowSize.y = windowSizeH;
        config.windowSize = windowSize;
	}
    // Check that everything worked out okay
    if (window_ == nullptr)
    {
        logDebug("[Error] Unable to create window\n");
        logDebug(SDL_GetError());
        return;
    }
}

void sdl::SdlWindow::InitImGui()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("InitSdlImGui");
#endif
// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;

    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Keyboard Gamepad
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking

    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    //io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    //io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
   
    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();
}


void sdl::SdlWindow::ImguiNewFrame()
{
    ImGui_ImplSDL2_NewFrame(window_);
    ImGui::NewFrame();
}


void sdl::SdlWindow::Destroy()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DestroySdlWindow");
#endif
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    // Destroy our window
    SDL_DestroyWindow(window_);
}

void sdl::SdlWindow::SwapBuffer()
{

}

void sdl::SdlWindow::RenderUi()
{
    ImGui::Render();
}

}