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
#include <sdl_engine/sdl_engine.h>

#include "mathematics/hash.h"

namespace neko::aer
{
class AerEngine;
class EditorToolInterface : public SystemInterface,
							public DrawImGuiInterface,
							public sdl::SdlEventSystemInterface
{
public:
	enum ToolType : std::uint8_t
	{
		NONE = 0,
		LOGGER,
        HIERARCHY,
    	INSPECTOR,
		SCENE_LOADER,
		LIGHT_CONTROLLER
	};

	explicit EditorToolInterface(AerEngine& engine);

	void Init() override {}
	void Update(seconds) override {}
	void Destroy() override {}

	void OnEvent(const SDL_Event&) override {}

	[[nodiscard]] virtual StringHash GetId() const { return HashString(GetName()); }
	[[nodiscard]] virtual ToolType GetType() const = 0;
	[[nodiscard]] virtual std::string_view GetName() const = 0;

	bool isVisible = true;

protected:
	AerEngine& engine_;
};
}
