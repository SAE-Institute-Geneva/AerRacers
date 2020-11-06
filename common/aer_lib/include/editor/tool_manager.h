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
#include "editor/EditorToolInterface.h"

namespace neko::aer //TODOCR(Luca@Dylan)  Apply Code Style
{
class AerEngine;

class EditorToolManager final : public SystemInterface, public DrawImGuiInterface, public sdl::SdlEventSystemInterface
{
public:
	explicit EditorToolManager(AerEngine& engine);
	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;
	void DrawImGui() override;
	void OnEvent(const SDL_Event& event) override;
	template<typename T, ToolType Type>
	void AddEditorTool()
	{
		auto newTool = std::make_unique<T>(Type);
		newTool->Init();
		tools_.push_back(std::move(newTool));
	}
private:
	void DrawList();
	
	AerEngine& engine_;
	//std::vector<Tool*> tools_; //TODOCR(Luca@Dylan)  Nomenclature
	std::vector<std::unique_ptr<EditorToolInterface>> tools_;
	bool aboutVisible_ = false;
};
}
