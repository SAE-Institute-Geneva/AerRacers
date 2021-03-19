#include "aer/editor/tool/logger.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
Logger::Logger(AerEngine& engine, ToolType type, int id, std::string_view name)
   : EditorToolInterface(engine, type, id, name)
{}

void Logger::Init() {}

void Logger::Update(seconds) {}

void Logger::Destroy() {}

void Logger::DrawImGui()
{
	if (isVisible)
	{
		// nbrLogs contains the quantity of logs
		int nbrLog = Log::get().GetLogs().size();
		const int nbrLogDisplayMax = static_cast<const int>(
			ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing());

		// Beginning of the Logger window
		const std::string name = std::string(GetName()) + "##" + std::to_string(GetId());
		if (!ImGui::Begin(name.c_str(), &isVisible))
		{
			ImGui::End();
		}
		else
		{
			//Removes all Logs
			if (ImGui::Button("Clear"))
			{
				ClearLogs();
				nbrLog = 0;
			}

			//Enables/Disables automatic scroll
			ImGui::SameLine();
			if (ImGui::Checkbox("Auto Scrolling", &autoScroll_)) {}
			ImGui::Separator();

			ImGui::BeginChild("Logs");
			if (autoScroll_)
			{
				// Calculation of the y-position in pixel
				posY_ = nbrLog - nbrLogDisplayMax;
				if (posY_ < 0) { posY_ = 0; }
			}
			else
			{
				posY_ =
					static_cast<int>(ImGui::GetScrollY() / ImGui::GetTextLineHeightWithSpacing());
				ImGui::SetCursorPos(
					{0.0f, static_cast<float>(posY_) * ImGui::GetTextLineHeightWithSpacing()});
			}

			// Displays logs by severity
			if (nbrLog != 0)
			{
				for (int i = posY_; i < posY_ + nbrLogDisplayMax; i++)
				{
					if (i < nbrLog)
					{
						LogMessage log = Log::get().GetLogs()[i];
						switch (log.type)
						{
							case LogType::DEBUG_:
								ImGui::TextColored(Color::blue, "%s", log.log.c_str());
								break;
							case LogType::INFO: ImGui::Text("%s", log.log.c_str()); break;
							case LogType::WARNING:
								ImGui::TextColored(Color::yellow, "%s", log.log.c_str());
								break;
							case LogType::ERROR_:
								ImGui::TextColored(Color::orange, "%s", log.log.c_str());
								break;
							case LogType::CRITICAL:
								ImGui::TextColored(Color::red, "%s", log.log.c_str());
								break;
							default: break;
						}
					}
				}
			}
			else
			{
				ImGui::SetScrollY(0.0f);
			}

			if (!autoScroll_)
			{
				// Scroll Space
				ImGui::SetCursorPos(
					{0.0f, static_cast<float>(nbrLog) * ImGui::GetTextLineHeightWithSpacing()});
				ImGui::Text("");
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}
}

void Logger::ClearLogs() { Log::get().ClearLogs(); }

void Logger::OnEvent(const SDL_Event&) {}
}    // namespace neko::aer
