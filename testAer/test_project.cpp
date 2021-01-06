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
 Date : 03.11.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "aer_engine.h"
#include "editor/tool/project.h"
#include "editor/editor_tool_manager.h"

class SimulateProject : public neko::SystemInterface
{
public:

	SimulateProject(neko::aer::AerEngine& engine)
		: engine_(engine)
	{
		if (engine.GetMode() != neko::aer::ModeEnum::GAME)
		{
			toolManager_ = std::make_unique<neko::aer::EditorToolManager>(engine_);
			engine_.RegisterSystem(*toolManager_);
			engine_.RegisterOnDrawUi(*toolManager_);
			engine_.RegisterOnEvent(*toolManager_);
			toolManager_->AddEditorTool<neko::aer::Project, neko::aer::EditorToolInterface::ToolType::PROJECT>();
			toolLaunched = true;
		}
	}

	void Init() override
	{
	}

	void Update(neko::seconds dt) override
	{

		  if(fileOpened) {
			  engine_.Stop();
		  }
		if (toolLaunched == true)
		{
			fileOpened = toolManager_->OpenFile("./../../data/testFile/LICENSE.txt");
		}
	}

	void Destroy() override
	{
	}

	void HasSucceed() const
	{
		EXPECT_TRUE(toolLaunched);
		EXPECT_TRUE(fileOpened);
	}

private:
	std::unique_ptr<neko::aer::EditorToolManager> toolManager_;
	bool toolLaunched = false;
	bool fileOpened = false;

	neko::aer::AerEngine& engine_;
};

TEST(Tool, TestToolProject)
{
	neko::Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = neko::Vec2u(1400, 900);

	neko::sdl::Gles3Window window;
	neko::gl::Gles3Renderer renderer;
	neko::aer::AerEngine engine(&config, neko::aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);

	SimulateProject simulateProject(engine);
	engine.RegisterSystem(simulateProject);

	engine.Init();

	engine.EngineLoop();

	simulateProject.HasSucceed();
}