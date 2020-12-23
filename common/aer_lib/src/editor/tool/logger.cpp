#include <utils/file_utility.h>

#include "aer_engine.h"
#include "log.h"
#include "editor/tool/logger.h"

namespace neko::aer
{
Logger::Logger(AerEngine& engine, ToolType type, int id, std::string name)
	: EditorToolInterface(engine, type, id, name)
{
}

void Logger::Init()
{
}

void Logger::Update(seconds dt)
{
}

void Logger::Destroy()
{
}

void Logger::DrawImGui()
{
	if (isVisible)
	{
		// nbrLogs contains the quantity of logs
		int nbrLog = Log::get().GetLogs().size();
		const int nbrLogDisplayMax =
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
			//Removes all Logs
			if (ImGui::Button("Clear"))
			{
				ClearLogs();
				nbrLog = 0;
			}

			//Enables/Disables automatic scroll
			ImGui::SameLine();
			if (ImGui::Checkbox("Auto Scrolling", &autoScroll_))
			{
			}
			ImGui::Separator();
#pragma endregion

#pragma region Body
			ImGui::BeginChild("Logs");
			if (autoScroll_)
			{
				// Calculation of the y-position in pixel
				posY_ = nbrLog - nbrLogDisplayMax;
				if (posY_ < 0) { posY_ = 0; }
			}
			else
			{
				posY_ = ImGui::GetScrollY() /
					ImGui::GetTextLineHeightWithSpacing();
				ImGui::SetCursorPos(
					{0, posY_ * ImGui::GetTextLineHeightWithSpacing()});
			}

			// Displays logs by severity
			if (nbrLog != 0)
			{
				for (size_t i = posY_; i < posY_ + nbrLogDisplayMax; i++)
				{
					if (i < nbrLog)
					{
						LogMessage log = Log::get().GetLogs()[i];
						switch (log.type)
						{
							case LogType::DEBUG_:
								ImGui::TextColored(kBlue_, log.log.c_str());
								break;
							case LogType::INFO:
								ImGui::Text(log.log.c_str());
								break;
							case LogType::WARNING:
								ImGui::TextColored(kYellow_, log.log.c_str());
								break;
							case LogType::ERROR_:
								ImGui::TextColored(kOrange_, log.log.c_str());
								break;
							case LogType::CRITICAL:
								ImGui::TextColored(kRed_, log.log.c_str());
								break;
							default:
								break;
						}
					}
				}
			}
			else { ImGui::SetScrollY(0); }

			if (!autoScroll_)
			{
				// Scroll Space
				ImGui::SetCursorPos(
					{0, nbrLog * ImGui::GetTextLineHeightWithSpacing()});
				ImGui::Text("");
			}
			ImGui::EndChild();
			ImGui::End();
#pragma endregion
		}
	}
}


void Logger::ClearLogs() { Log::get().ClearLogs(); }

void Logger::OnEvent(const SDL_Event& event)
{
}
}
