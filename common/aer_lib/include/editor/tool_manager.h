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
#include <SDL_events.h>
#include <imgui.h>
#include <editor/tool.h>

namespace neko::aer
{
    class AerEngine;
    class ToolManager final : public SystemInterface {
    public:
        explicit ToolManager(AerEngine& engine);
        void Init() override;
        void Update(seconds dt) override;
        void DrawList();
        void Destroy() override;

        void LimitationWindow() {
            // LIMITATION POSITION
            // X
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::GetWindowPos().x < 0) {
                ImGui::SetWindowPos(ImVec2(0, ImGui::GetWindowPos().y));
            }

            if (ImGui::GetWindowPos().x + ImGui::GetWindowSize().x > io.DisplaySize.x) {
                ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - ImGui::GetWindowSize().x, ImGui::GetWindowPos().y));
            }
            // Y
            if (ImGui::GetWindowPos().y < 20) {
                ImGui::SetWindowPos(ImVec2(ImGui::GetWindowPos().x, 20));
            }

            if (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y > io.DisplaySize.y) {
                ImGui::SetWindowPos(ImVec2(ImGui::GetWindowPos().x, io.DisplaySize.y - ImGui::GetWindowSize().y));
            }
        }

        void LimitationDock() {
            
        }

        Tool* GetTool(TypeTool type);

    protected:
        AerEngine& engine_;
    private:
        std::vector<Tool*> tools_;
    };
}