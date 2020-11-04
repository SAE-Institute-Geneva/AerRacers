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
 */ //TODOCR(Luca@Dylan)  Wrong Header
#include <sdl_engine/sdl_engine.h>
#include <SDL_events.h>
#include <imgui.h>

namespace neko::aer //TODOCR(Luca@Dylan)  Apply Syntax Style
{
    enum class TypeTool { //TODOCR(Luca@Dylan)  enum class
        LOG,
        INSP,
    };
    const int headerSpace = 20; //TODOCR(Luca@Dylan) Why here ? + Nomenclature
    class Tool : public SystemInterface, public DrawImGuiInterface, public sdl::SdlEventSystemInterface {
    public:

        Tool(TypeTool type) : type(type) { } //TODOCR(Luca@Dylan)  explicit
        virtual void Init() override {}
        virtual void Update(seconds dt) override {}
        virtual void DrawImGui() override {}
        virtual void Destroy() override {}
        virtual void OnEvent(const SDL_Event& event) override {}

        void LimitationWindow() { //TODOCR(Luca@Dylan)  function can be const
            // LIMITATION POSITION
            ImGuiIO& io = ImGui::GetIO();
            // X
            if (ImGui::GetWindowPos().x < 0) {
                ImGui::SetWindowPos(ImVec2(0, ImGui::GetWindowPos().y));
            }

            if (ImGui::GetWindowPos().x + ImGui::GetWindowSize().x > io.DisplaySize.x) {
                ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - ImGui::GetWindowSize().x, ImGui::GetWindowPos().y));
            }
            // Y
            if (ImGui::GetWindowPos().y < headerSpace) {
                ImGui::SetWindowPos(ImVec2(ImGui::GetWindowPos().x, headerSpace));
            }

            if (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y > io.DisplaySize.y) {
                ImGui::SetWindowPos(ImVec2(ImGui::GetWindowPos().x, io.DisplaySize.y - ImGui::GetWindowSize().y));
            }
        }

        void LimitationDock() {
            //TODO (@Dylan)
        }
        
        TypeTool type; //TODOCR(Luca@Dylan) Move on private
        bool isVisible = false; 
    private:
        int id_;
    protected:
        ImGuiID dockspaceID;  //TODOCR(Luca@Dylan)  Move on private + Nomenclature
    };
}