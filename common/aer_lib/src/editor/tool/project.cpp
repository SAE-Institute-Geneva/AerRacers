#include <utilities/file_utility.h>

#include "editor/tool/project.h"

namespace neko::aer
{
Project::Project(ToolType type, int id, std::string name)
	: EditorToolInterface(type, id, name)
{
}

void Project::Init()
{
	// Todo Read Folder Data
}

void Project::Update(seconds dt)
{
}

void Project::Destroy()
{
}

void Project::DrawImGui()
{
	if (isVisible)
	{
		// nbrLogs contains the quantity of Resource
		const int nbrResource =  10;
		const int nbrResourceDisplayMax =
			ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing();

		// Beginning of the Logger window
		if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(),
		                  &isVisible))
		{
			ImGui::End();
		}
		else
		{
#pragma region Header
			//Todo Search
			ImGui::Separator();
#pragma endregion

#pragma region Body
			ImGui::BeginChild("Resources");

				posY_ = ImGui::GetScrollY() /
					ImGui::GetTextLineHeightWithSpacing();
				ImGui::SetCursorPos(
					{0, posY_ * ImGui::GetTextLineHeightWithSpacing()});

			// Displays Resource
			if (nbrResource != 0)
			{
				for (size_t i = posY_; i < posY_ + nbrResourceDisplayMax; i++)
				{
					if (i < nbrResource)
					{
						//Todo Display Resource
					}
				}
			}
			else {
			  ImGui::SetScrollY(0);
			}

			ImGui::SetCursorPos(
				{0, nbrResource * ImGui::GetTextLineHeightWithSpacing()});
			ImGui::Text("");

			ImGui::EndChild();
			ImGui::End();
#pragma endregion
		}
	}
}


void Project::OnEvent(const SDL_Event& event)
{
}
}
