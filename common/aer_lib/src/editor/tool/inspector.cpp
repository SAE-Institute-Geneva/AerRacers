///*
// MIT License
//
// Copyright (c) 2020 SAE Institute Switzerland AG
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// */
#include "editor/tool/inspector.h"

namespace neko::aer
{
    void Inspector::Init()
    {
    }

    void Inspector::Update(seconds dt)
    {

    }

    void Inspector::Destroy()
    {
    }


    void Inspector::DrawImGui()
    {

        if (isVisible) 
        {
            //Tool Logger
            ImGui::SetNextWindowPos(ImVec2(0, 400), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Inspector", &isVisible))
            {
                ImGui::End();
            }
            else
            {
             /*   ImGui::IsWindowDocked();
                #pragma region Header
                Separator
                ImGui::Separator();*/
                #pragma endregion
                #pragma region Body
                //ImGui::BeginChild("Body");

                ImGui::End();
                #pragma endregion 
            }
        }

    }

    void Inspector::OnEvent(const SDL_Event& event)
    {
     
    }

}
