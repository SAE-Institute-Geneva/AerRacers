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
    Logger::Logger(TypeTool type) : Tool(type) {
        Log::provide(this);
        logs_.reserve(CAPACITY_LOG);
    }
    
    void Logger::Init()
    {
        
    }

    void Logger::Update(seconds dt)
    {

    }

    void Logger::Destroy()
    {
        logs_.clear();
    }


    void Logger::DrawImGui()
    {
         
        if (isVisible) {
            //Number of Logs  
            int nbrLogs = logs_.size();
            int nbrLineMax = ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing();

            //Tool Logger
            if (!ImGui::Begin("Logger Tool", &isVisible))
            {
                ImGui::End();
            }
            else
            {
                #pragma region Header
                //Clear All Logs
                if (ImGui::Button("Clear")) {
                    logs_.clear();
                }

                //Auto Scrolling
                ImGui::SameLine();
                if (ImGui::Checkbox("Auto Scrolling", &autoScroll)) {
                    autoScroll != autoScroll;
                }

              
                ImGui::SameLine();
             
                ImGui::Text(("Counter Logs: " + std::to_string(nbrLogs)).c_str());

                //Separator
                ImGui::Separator();
                #pragma endregion
                #pragma region Body
                ImGui::BeginChild("Logs");
                if (autoScroll) {
                    pos_y = nbrLogs - nbrLineMax;
                    if (pos_y < 0) {
                        pos_y = 0;
                    }
                }
                else {
                    pos_y = ImGui::GetScrollY() / ImGui::GetTextLineHeightWithSpacing();
                    ImGui::SetCursorPos({ 0, (float)pos_y * ImGui::GetTextLineHeightWithSpacing() });
                }
              
                //Display
                if (nbrLogs != 0) {
                    for (size_t i = pos_y; i < pos_y + (nbrLineMax); i++)
                    {
                        if (i < nbrLogs) {
                            AerLog log = logs_[i];
                            switch (log.severity) {
                            case LogSeverity::DEBUG:
                                ImGui::TextColored(BLUE, log.msg.c_str());
                                break;
                            case LogSeverity::INFO:
                                ImGui::Text(log.msg.c_str());
                                break;
                            case LogSeverity::WARNING:
                                ImGui::TextColored(YELLOW, log.msg.c_str());
                                break;
                            case LogSeverity::ERROOR:
                                ImGui::TextColored(ORANGE, log.msg.c_str());
                                break;
                            case LogSeverity::CRITICAL:
                                ImGui::TextColored(RED, log.msg.c_str());
                                break;
                            }
                        }
                    }
                }
                else {
                    ImGui::SetScrollY(0);
                }

                if (!autoScroll) {
                    ImGui::SetCursorPos({ 0, (float)nbrLogs * ImGui::GetTextLineHeightWithSpacing() });
                    ImGui::Text("");
                }
                ImGui::EndChild();
                ImGui::End();
                #pragma endregion 
            }
        }
        LimitationWindow();
    }

    void Logger::Log(LogSeverity severity, const std::string msg)
    {
        // current date/time based on current system
        time_t now = time(0);
        // convert now to string form
        tm* ltm = localtime(&now);

        std::string time_log = "[" + std::to_string(ltm->tm_hour) + ":"
            + std::to_string(ltm->tm_min) + ":"
            + std::to_string(ltm->tm_sec) + "]";

        AerLog log;
        log.severity = severity;

        switch (log.severity)
        {
        case LogSeverity::DEBUG: 
            log.msg = time_log + " [Debug] " + msg;
            break;
        case LogSeverity::INFO:
            log.msg = time_log + " [Info] " + msg;
            break;
        case LogSeverity::WARNING:
            log.msg = time_log + " [Warning] " + msg;
            break;
        case LogSeverity::ERROOR:
            log.msg = time_log + " [Error] " + msg;
            break;
        case LogSeverity::CRITICAL:
            log.msg = time_log + " [Critical] " + msg;
            break;

        default:
            break;
        }

        logs_.emplace_back(log);
    }

    void Logger::OnEvent(const SDL_Event& event) { }

    void DebugLog(std::string msg) {   
        Log::get().Log(LogSeverity::DEBUG, msg);
    }

    void InfoLog(std::string msg) {
        Log::get().Log(LogSeverity::INFO, msg);
    }

    void WarningLog(std::string msg) {
        Log::get().Log(LogSeverity::WARNING, msg);
    }

    void ErrorLog(std::string msg) {
        Log::get().Log(LogSeverity::ERROOR, msg);
    }

    void CriticalLog(std::string msg) {
        Log::get().Log(LogSeverity::CRITICAL, msg);
    }
}
