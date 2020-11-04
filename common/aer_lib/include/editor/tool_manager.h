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
 */ //TODOCR(Luca@Dylan)  Use good header 
#include <imgui.h>
#include <editor/tool.h> //TODOCR(Luca@Dylan) Use coding style include ""

namespace neko::aer //TODOCR(Luca@Dylan)  Apply Code Style
{
    class AerEngine;
    class ToolManager final : public SystemInterface, public DrawImGuiInterface, public sdl::SdlEventSystemInterface {
    public:
        explicit ToolManager(AerEngine& engine);
        void Init() override;

        void InitTools();
        void Update(seconds dt) override;
        void DrawList();
        void Destroy() override;
        void DrawImGui() override;
        void OnEvent(const SDL_Event& event) override;
        Tool* GetTool(TypeTool type);

    protected: //TODOCR(Luca@Dylan) Why protected 
        AerEngine& engine_;
        std::vector<Tool*> tools_; //TODOCR(Luca@Dylan)  Nomenclature
    private:  
        bool about_IsVisible = false; //TODOCR(Luca@Dylan)  Nomenclature
    };
}