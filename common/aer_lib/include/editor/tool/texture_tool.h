#pragma once
/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person
 of this software and associated documentation files (the "So
 in the Software without restriction, including without limit
 to use, copy, modify, merge, publish, distribute, sublicense
 copies of the Software, and to permit persons to whom the So
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KI
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERC
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGE
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWI
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHE
 SOFTWARE.

 Author : Bryan Carmona
 Co-Author :
 Date : 08.12.2020
---------------------------------------------------------- */
#include <SDL_events.h>
#include "editor/editor_tool_interface.h"

namespace neko::aer {

    class TextureInterface final : public EditorToolInterface {
    public:
        explicit TextureInterface(ToolType type, int id, std::string name);

        void Init() override;

        void Update(seconds dt) override;

        void DrawImGui() override;

        void Destroy() override;

        void OnEvent(const SDL_Event& event) override;
    };
}