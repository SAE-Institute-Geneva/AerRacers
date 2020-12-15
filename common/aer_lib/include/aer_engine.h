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
Last Modif: 13.10.2020
---------------------------------------------------------- */

#include "sdl_engine/sdl_engine.h"

#include "draw_system.h"
#include "inputs_binding_manager.h"
#include "log.h"
#include "editor/editor_tool_manager.h"

namespace neko::aer {

enum class ModeEnum : std::uint8_t {
    EDITOR = 0,
    GAME = 1,
    TEST = 2
};

class AerEngine final : public sdl::SdlEngine {
public:
    explicit AerEngine(
        const FilesystemInterface& filesystem,
        Configuration* config = nullptr,
        ModeEnum mode = ModeEnum::EDITOR);

    void Init() override;

    void Destroy() override;

    void ManageEvent() override;

    void GenerateUiFrame() override;

    ModeEnum GetMode() const
    {
        return mode_;
    }

private:
    ModeEnum mode_;

    DrawSystem drawSystem_;
    std::unique_ptr<LogManager> logManager_;
    std::unique_ptr<InputBindingManager>  bindedInputManager_;

    EditorToolManager toolManager_;
};
}
