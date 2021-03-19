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
#include "px/physics_callbacks.h"

#include "aer/editor/editor_tool_interface.h"

namespace neko::aer
{
class SceneManager;
class SceneLoader final : public EditorToolInterface, public physics::FixedUpdateInterface
{
public:
	explicit SceneLoader(AerEngine& engine);

	void Init() override;

	void DrawImGui() override;
	void LoadSceneFiles();

	void FixedUpdate(seconds dt) override;

	[[nodiscard]] std::string_view GetName() const override { return "Scene Loader"; }
	[[nodiscard]] ToolType GetType() const override { return SCENE_LOADER; }

private:
	SceneManager& sceneManager_;

	std::string filepath_;

	std::vector<std::string> scenesPaths_;
	unsigned selectedSceneIndex_ = 0;
	bool toSave_                 = false;

	// Not defined
	void Update(seconds) override {}
	void Destroy() override {}
	void OnEvent(const SDL_Event&) override {}
};
}    // namespace neko::aer