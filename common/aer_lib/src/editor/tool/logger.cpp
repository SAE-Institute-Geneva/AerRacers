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
#include "editor/tool/logger.h"

namespace neko::aer
{
    Logger* Logger::instance = nullptr;

    void Logger::Init()
    {
        instance = this;
        
     
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
                    instance->logs_.clear();
                }

                ImGui::SameLine(80);
                if (ImGui::Checkbox("Auto Scrolling", &scrollToBottom)) {
                    scrollToBottom != scrollToBottom;
                }

                ImGui::SameLine(220);
                ImGui::Text(("Counter Logs: " + std::to_string(instance->logs_.size())).c_str());

                //Separator
                ImGui::Separator();
#pragma endregion
#pragma region Body
                ImGui::BeginChild("Logger");

                int nbrLineMax = ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing();

                if (scrollToBottom) {
                    pos_y = instance->logs_.size() - nbrLineMax;
                    if (pos_y < 0) {
                        pos_y = 0;
                    }
                }
                else {
                    pos_y = ImGui::GetScrollY() / ImGui::GetTextLineHeightWithSpacing();
                    ImGui::SetCursorPos({ 0, (float)pos_y * ImGui::GetTextLineHeightWithSpacing() });
                }
              

                if (instance->logs_.size() != 0) {
                    for (size_t i = pos_y; i < pos_y + (nbrLineMax); i++)
                    {
                        if (i < instance->logs_.size()) {
                            AerLog log = instance->logs_[i];
                            switch (log.severity) {
                            case LogSeverity::DEBUG:
                                ImGui::TextColored(ImVec4(0.5f, 0.5f, 1, 1), log.msg.c_str());
                                break;
                            case LogSeverity::INFO:
                                ImGui::Text(log.msg.c_str());
                                break;
                            case LogSeverity::WARNING:
                                ImGui::TextColored(ImVec4(1, 1, 0, 1), log.msg.c_str());
                                break;
                            case LogSeverity::ERROR:
                                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), log.msg.c_str());
                                break;
                            case LogSeverity::CRITICAL:
                                ImGui::TextColored(ImVec4(1, 0, 0, 1), log.msg.c_str());
                                break;
                            }
                        }
                    }
                }
                else {
                    ImGui::SetScrollY(0);
                }

                if (!scrollToBottom) {
                    ImGui::SetCursorPos({ 0, (float)instance->logs_.size() * ImGui::GetTextLineHeightWithSpacing() });
                    ImGui::Text("");
                }
                ImGui::EndChild();
                ImGui::End();
#pragma endregion 
            }
        }
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

        AerLog log;
        switch (severity_log)
        {
        case 0:
            log.severity = LogSeverity::DEBUG;
            log.msg = time_log + " Debug: " + msg_log;
            break;
        case 1:
            log.severity = LogSeverity::INFO;
            log.msg = time_log + " Info: " + msg_log;
            break;
        case 2:
            log.severity = LogSeverity::WARNING;
            log.msg = time_log + " Warning: " + msg_log;
            break;
        case 3:
            log.severity = LogSeverity::ERROR;
            log.msg = time_log + " Error: " + msg_log;
            break;
        case 4:
            log.severity = LogSeverity::CRITICAL;
            log.msg = time_log + " Critical:" + msg_log;
            break;

        default:
            break;
        }
        logs_.emplace_back(log);
    }

    void Logger::OnEvent(const SDL_Event& event)
    {
        
    }

    void DebugLog(std::string msg) {   
        Logger::get()->AddLog(msg, 0);
    }

    void InfoLog(std::string msg) {
        Logger::get()->AddLog(msg, 1);
    }

    void WarningLog(std::string msg) {
        Logger::get()->AddLog(msg, 2);
    }

    void ErrorLog(std::string msg) {
        Logger::get()->AddLog(msg, 3);
    }


    void CriticalLog(std::string msg) {
        Logger::get()->AddLog(msg, 4);
    }
}
