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

 Author : Floreau Luca
 Co-Author :
 Date : 22.01.2021
---------------------------------------------------------- */
#include "aer/editor/editor_tool_interface.h"

namespace neko::aer
{
class LightController final : public EditorToolInterface
{
public:
	explicit LightController(AerEngine& engine);

	void DrawImGui() override;

	[[nodiscard]] std::string_view GetName() const override { return "Light Controller"; }
	[[nodiscard]] ToolType GetType() const override { return LIGHT_CONTROLLER; }

	// Not defined
	void Init() override {}
	void Update(seconds) override {}
	void Destroy() override {}
	void OnEvent(const SDL_Event&) override {}

private:
};
}    // namespace neko::aer