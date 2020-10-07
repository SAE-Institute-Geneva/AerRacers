#pragma once
/* ----------------------------------------------------
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
 Date : 29.09.2020
---------------------------------------------------------- */

#include "sdl_engine/sdl_engine.h"

#include "draw_system.h"
#include "Editor/tools_system.h"

namespace neko::aer
{
using ToolsMask = std::uint8_t;

enum ToolsFlags : std::uint8_t
{
	EMPTY = 1u << 0u,
	LOGGER = 1u << 1u,
	INSPECTOR = 1u << 2u,
	ENTITY_VIEWER = 1u << 3u,
	PROFILER = 1u << 4u
};

class AerEngine final : public sdl::SdlEngine
{
public:
	explicit AerEngine(Configuration* config = nullptr, ToolsMask toolsMask = 0);

	void Init() override;

	void Destroy() override;

	void ManageEvent() override;

	void GenerateUiFrame() override;

private:
	DrawSystem drawSystem_;
	ToolsSystem toolsSystem_;
};
}
