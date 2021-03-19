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

 Author : Dylan von Arx
 Co-Author :
 Date : 11.11.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>

#ifdef NEKO_GLES3
	#include <gl/gles3_window.h>
	#include <gl/graphics.h>
	#include <sdl_engine/sdl_engine.h>
	#include <sdl_engine/sdl_input.h>

	#include "aer/aer_engine.h"
	#include "aer/editor/tool/logger.h"

namespace neko::aer
{
class TestToolInterface : public EditorToolInterface
{
public:
	explicit TestToolInterface(
		AerEngine& engine, ToolType type, int id, std::string name)
	   : EditorToolInterface(engine, type, id, name)
	{
		color_       = ImVec4(std::rand() % 2, std::rand() % 2, std::rand() % 2, 1);
		counterTime_ = -std::rand() % 2;
	}

	void Init() override {}

	void Update(seconds dt) override
	{
		counterTime_ += dt.count();
		if (counterTime_ >= kTimeToWait_)
		{
			counterTime_ = -std::rand() % 2;
			isVisible    = !isVisible;
		}
	}

	void DrawImGui() override
	{
		if (isVisible)
		{
			// Beginning of the Test window
			if (!ImGui::Begin((std::string(GetName()) + "##" + std::to_string(GetId())).c_str(), &isVisible))
			{
				ImGui::End();
			}
			else
			{
				// Window Label
				if (ImGui::IsWindowDocked())
				{
					ImGui::Text(GetName());
					ImGui::Separator();
				}

				ImGui::End();
			}
		}
	}

	void OnEvent(const SDL_Event&) override {}

	void Destroy() override {}

private:
	ImVec4 color_;
	float counterTime_       = 0.0f;
	const float kTimeToWait_ = 0.6f;
};

class SimulateEditor : public SystemInterface
{
public:
	SimulateEditor(AerEngine& engine) : engine_(engine)
	{
		toolManager_ = std::make_unique<EditorToolManager>(engine_);
		engine_.RegisterSystem(*toolManager_);
		engine_.RegisterOnDrawUi(*toolManager_);
		engine_.RegisterOnEvent(*toolManager_);

		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();

		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();

		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();

		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();

		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
		toolManager_
			->AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();

		if (toolManager_->GetNumberTools() == kNbrTool_) { allToolInit_ = true; }
	}

	void Init() override {}

	void Update(seconds dt) override
	{
		counterTime_ += dt.count();
		if (counterTime_ >= kTimeToWait_)
		{
			testSuccess_ = true;
			engine_.Stop();
		}
	}

	void Destroy() override {}

	void HasSucceed() const
	{
		EXPECT_TRUE(allToolInit_);
		EXPECT_TRUE(testSuccess_);
	}

private:
	std::unique_ptr<EditorToolManager> toolManager_;
	bool allToolInit_ = false;
	bool testSuccess_ = false;

	AerEngine& engine_;

	float counterTime_       = 0.0f;
	const float kTimeToWait_ = 2.0f;
	const int kNbrTool_      = 10;
};

TEST(Editor, TestEditor)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::TEST);

	engine.SetWindowAndRenderer(&window, &renderer);

	engine.Init();

	SimulateEditor simulateEditor(engine);
	engine.RegisterSystem(simulateEditor);

	engine.EngineLoop();

	simulateEditor.HasSucceed();
}
}    // namespace aer
#endif
