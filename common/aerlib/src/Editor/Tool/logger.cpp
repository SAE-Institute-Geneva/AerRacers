///*
// MIT License
//
// Copyright (c) 2020 SAE Institute Switzerland AG
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// */
#include "Editor/Tool/logger.h"

namespace neko::aer
{
    void Logger::Init()
    {
        logs_.reserve(100000);
    }

    void Logger::Update(seconds dt)
    {

    }

    void Logger::Destroy()
    {
        //ImGui::
        logs_.clear();
    }


    void Logger::DrawImGui()
    {

        if (isVisible) {
            //Tool Logger
            ImGui::SetNextWindowPos(ImVec2(0, 400), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Logger Tool", &isVisible))
            {
                ImGui::End();
            }
            else
            {
                ImGui::IsWindowDocked();
#pragma region Header
                if (ImGui::Button("Clear")) {
                    logs_.clear();
                }

                ImGui::SameLine(80);
                if (ImGui::Checkbox("Auto Scrolling", &scrollToBottom)) {
                    scrollToBottom != scrollToBottom;
                }

                ImGui::SameLine(220);
                ImGui::Text(("Counter Logs: " + std::to_string(logs_.size())).c_str());

                //Separator
                ImGui::Separator();
#pragma endregion
#pragma region Body
                ImGui::BeginChild("Logger");



                int nbrLineMax = ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing();

                if (scrollToBottom) {
                    scroll_Y = logs_.size() - nbrLineMax;
                    if (scroll_Y < 0) {
                        scroll_Y = 0;
                    }
                }
                else {
                    scroll_Y = ImGui::GetScrollY() / ImGui::GetTextLineHeightWithSpacing();
                    ImGui::SetCursorPos({ 0, (float)scroll_Y * ImGui::GetTextLineHeightWithSpacing() });
                }

                if (logs_.size() != 0) {
                    for (size_t i = scroll_Y; i < scroll_Y + (nbrLineMax); i++)
                    {
                        if (i < logs_.size()) {
                            switch (logs_[i].log_severity) {
                            case 0:
                                ImGui::TextColored(ImVec4(0.5f, 0.5f, 1, 1), logs_[i].log_msg.c_str());
                                break;
                            case 1:
                                ImGui::Text(logs_[i].log_msg.c_str());
                                break;
                            case 2:
                                ImGui::TextColored(ImVec4(1, 1, 0, 1), logs_[i].log_msg.c_str());
                                break;
                            case 3:
                                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), logs_[i].log_msg.c_str());
                                break;
                            case 4:
                                ImGui::TextColored(ImVec4(1, 0, 0, 1), logs_[i].log_msg.c_str());
                                break;
                            }
                        }
                    }
                }
                else {
                    ImGui::SetScrollY(0);
                }

                if (!scrollToBottom) {
                    ImGui::SetCursorPos({ 0, (float)logs_.size() * ImGui::GetTextLineHeightWithSpacing() });
                    ImGui::Text("");
                }
                ImGui::EndChild();
                ImGui::End();
#pragma endregion 
            }
        }
        AddLog(std::to_string(ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing()).c_str(), rand() % 5);

    }


    void Logger::AddLog(std::string msg_log, int severity_log)
    {
        // current date/time based on current system
        time_t now = time(0);
        // convert now to string form
        tm* ltm = localtime(&now);

        std::string time_log = "[" + std::to_string(ltm->tm_hour) + ":"
            + std::to_string(ltm->tm_min) + ":"
            + std::to_string(ltm->tm_sec) + "]";

        MyLog new_log;
        switch (severity_log)
        {
        case 0:
            new_log.log_severity = 0;
            new_log.log_msg = time_log + " Debug: " + msg_log;
            break;
        case 1:
            new_log.log_severity = 1;
            new_log.log_msg = time_log + " Info: " + msg_log;
            break;
        case 2:
            new_log.log_severity = 2;
            new_log.log_msg = time_log + " Warning: " + msg_log;
            break;
        case 3:
            new_log.log_severity = 3;
            new_log.log_msg = time_log + " Error: " + msg_log;
            break;
        case 4:
            new_log.log_severity = 4;
            new_log.log_msg = time_log + " Critical:" + msg_log;
            break;

        default:
            break;
        }
        logs_.emplace_back(new_log);
    }

    void Logger::OnEvent(const SDL_Event& event)
    {
     
    }

}
