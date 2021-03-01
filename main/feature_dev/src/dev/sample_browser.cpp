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
#include <imgui.h>

#include "dev/sample_browser.h"

#include "programs/split_screen/split_screen.h"

namespace neko::dev
{
void SampleBrowser::Init()
{
	RegisterRenderProgram("Split Screen", std::make_unique<SplitScreen>());

#ifndef NEKO_SAMETHREAD
	Job initJob {[this]() { programs_[currentProgramIndex_]->Init(); }};
	BasicEngine::GetInstance()->ScheduleJob(&initJob, JobThreadType::RENDER_THREAD);
	initJob.Join();
#else
	programs_[currentProgramIndex_]->Init();
#endif
}

void SampleBrowser::Update(seconds dt)
{
	programs_[currentProgramIndex_]->Update(dt);
	RendererLocator::get().Render(programs_[currentProgramIndex_].get());
}

void SampleBrowser::DrawImGui()
{
	using namespace ImGui;
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = GetMainViewport();
	SetNextWindowPos(viewport->GetWorkPos());
	SetNextWindowSize(viewport->GetWorkSize());
	SetNextWindowViewport(viewport->ID);
	PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
	               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	Begin("Showroom", reinterpret_cast<bool*>(true), windowFlags);
	{
		PopStyleVar();
		PopStyleVar(2);

		dockspaceId_ = GetID("ShowroomDockSpace");
		DockSpace(dockspaceId_, ImVec2(0.0f, 0.0f), DockspaceFlags);
		BeginMenuBar();
		{
			if (BeginMenu("Program Browser"))
			{
				for (std::size_t n = 0; n < programsNames_.size(); n++)
				{
					const bool selected = currentProgramIndex_ == n;
					Selectable(("##" + programsNames_[n]).c_str(), selected); SameLine();
					if (MenuItem(programsNames_[n].c_str()))
					{
						SwitchToProgram(n);
					}
				}

				EndMenu();
			}
			EndMenuBar();
		}
		End();
	}

	programs_[currentProgramIndex_]->DrawImGui();
}

void SampleBrowser::Destroy() { programs_[currentProgramIndex_]->Destroy(); }

void SampleBrowser::SwitchToProgram(size_t index)
{
	const auto previousIndex = currentProgramIndex_;
	currentProgramIndex_     = index;
	programs_[previousIndex]->Destroy();
	programs_[currentProgramIndex_]->Init();
}

void SampleBrowser::OnEvent(const SDL_Event& event)
{
	programs_[currentProgramIndex_]->OnEvent(event);
}

size_t SampleBrowser::RegisterRenderProgram(
	std::string_view name, std::unique_ptr<SampleProgram> program)
{
	const auto index = programs_.size();
	programsNames_.emplace_back(name.data());
	programs_.push_back(std::move(program));
	return index;
}
}    // namespace neko::dev