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
#include <sdl_engine/sdl_engine.h>
#include <SDL_events.h>
#include <imgui.h>

namespace neko::aer
{
    class Tool : public SystemInterface, public DrawImGuiInterface, public sdl::SdlEventSystemInterface {
        /**
         * \brief Executed on the render thread
         */
        virtual void Init() override {

        }
        /**
         * \brief Executed on the main thread
         */
        virtual void Update(seconds dt) override {

        }
        /**
         * \brief Executed on the render thread
         */
        virtual void DrawImGui() override {

        }
        /**
         * \brief Executed on the render thread
         */
        virtual void Destroy() override {

        }
        /**
         * \brief Executed on the main thread
         */
        virtual void OnEvent(const SDL_Event& event) override {

        }
    public:
        bool isVisible = true;
    private:
        int id_;
    };
}