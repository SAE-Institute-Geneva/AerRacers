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
 */
#include "editor/editor.h"
#include "aer_engine.h"


namespace neko::aer
{
#pragma region Editor
	Editor::Editor(AerEngine& engine) : engine_(engine)
	{
	}


	void Editor::Init()
	{
		tool_manager = new ToolManager(engine_);
		tool_manager->Init();
		engine_.RegisterSystem(*tool_manager);
	}

	void Editor::Update(seconds dt)
	{
	}

	void Editor::Destroy()
	{
		tool_manager->Destroy();
	}


	void Editor::DrawImGui()
	{
		ImGuiIO io = ImGui::GetIO();
		
		//Editor Menu
		bool my_tool_active;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Tools"))
			{
				tool_manager->DrawList();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About Neko")) {
					about_IsVisible = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (about_IsVisible) {
			ImGui::Begin("About Aer Editor", &about_IsVisible);
			ImGui::Text("Welcome to");
			ImGui::Text((engine_.config.windowName).c_str());

			ImGui::Text("Neko is a 3D game engine based on SDL2 and OpenGL ES 3.0 that works on Desktop,");
			ImGui::Text("WebGL2 and Nintendo Switch (port to Android and iOS possible) used at SAE Institute Geneva.");
			ImGui::Text("");
			ImGui::Text("Copyright(c) 2020 SAE Institute Switzerland AG");
			ImGui::End();
		}
	}

	void Editor::OnEvent(const SDL_Event& event)
	{
		
	}

#pragma endregion
}
