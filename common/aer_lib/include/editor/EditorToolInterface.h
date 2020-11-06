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

 Author : Dylan von Arx
 Co-Author :
 Date : 03.11.2020
---------------------------------------------------------- */
#include <imgui.h>
#include <sdl_engine/sdl_engine.h>
// TODO RENAME FILE

namespace neko::aer //TODOCR(Luca@Dylan)  Apply Syntax Style
{
    enum class ToolType {
    	NONE,
        LOG, // TODO
        INSP, // TODO
    };

	// ToolInterface.
    class EditorToolInterface : public SystemInterface, public DrawImGuiInterface, public sdl::SdlEventSystemInterface {
    public:

        explicit EditorToolInterface(ToolType type) : type(type) { }

        bool isVisible = false;
        ToolType type = ToolType::NONE;
    protected:

        // Limit window move
        void LimitationWindow() const
        {

            ImGuiIO& io = ImGui::GetIO();
            // X
            if (ImGui::GetWindowPos().x < 0) {
                ImGui::SetWindowPos(ImVec2(0, ImGui::GetWindowPos().y));
            }

            if (ImGui::GetWindowPos().x + ImGui::GetWindowSize().x > io.DisplaySize.x) {
                ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - ImGui::GetWindowSize().x, ImGui::GetWindowPos().y));
            }
            // Y
            if (ImGui::GetWindowPos().y < headerSpace_) {
                ImGui::SetWindowPos(ImVec2(ImGui::GetWindowPos().x, headerSpace_));
            }

            if (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y > io.DisplaySize.y) {
                ImGui::SetWindowPos(ImVec2(ImGui::GetWindowPos().x, io.DisplaySize.y - ImGui::GetWindowSize().y));
            }
        }
    private:
        const int headerSpace_ = 20; // In Pixel
    };
}