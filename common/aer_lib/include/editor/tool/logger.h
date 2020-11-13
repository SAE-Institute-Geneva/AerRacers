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
 
 Author : Canas Simon
 Co-Author : Dylan von Arx
 Date : 13.10.2020
---------------------------------------------------------- */
#include <mutex>
#include <SDL_events.h>

#include "editor/editor_tool_interface.h"

namespace neko::aer {

class Logger final : public EditorToolInterface {
public:
    explicit Logger(ToolType type, int id, std::string name);

    void Init() override;

    void Update(seconds dt) override;

    void DrawImGui() override;

    void Destroy() override;

    void OnEvent(const SDL_Event& event) override;

    /**
    * \brief Deletes all logs 
    */
    static void ClearLogs();

private:
    int posY_ = 0;
    bool autoScroll_ = true;

    //COLOR
    const ImVec4 kBlue_ = {0.5f, 0.5f, 1, 1};
    const ImVec4 kYellow_ = {1, 1, 0, 1};
    const ImVec4 kOrange_ = {1, 0.5f, 0, 1};
    const ImVec4 kRed_{1, 0, 0, 1};
};

}
