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
#include "aer_engine.h"
#include "editor/tool_manager.h"
#include <editor/tool/logger.h>

namespace neko::aer
{
    ToolManager::ToolManager(AerEngine& engine) : engine_(engine)
    {
    }

    void ToolManager::Init()
    {
        tools_.push_back(new Logger(TypeTool::LOG));
        engine_.RegisterSystem(*tools_.back());
        engine_.RegisterOnEvent(*tools_.back());
        engine_.RegisterOnDrawUi(*tools_.back());
    }

    void ToolManager::Update(seconds dt)
    {

    }

    void ToolManager::Destroy()
    {
        for each (Tool * tool in tools_)
        {
            tool->Destroy();
        }
    }


    void ToolManager::DrawList()
    {
        if (ImGui::MenuItem("Logger", "Ctrl+L")) {
            GetTool(TypeTool::LOG)->isVisible = true;
        }
    }


    Tool* ToolManager::GetTool(TypeTool type) {
        for each (Tool * tool in tools_)
        {
            if (tool->type == type) {
                return tool;
            }
        }
        ErrorLog("Tool not found");
        return nullptr;
    }

}
