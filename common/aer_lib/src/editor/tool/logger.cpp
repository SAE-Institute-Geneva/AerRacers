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
            for (size_t i = 0; i < 10000; i++)
            {
                InfoLog("FUCK");
            }
           
        }
        if (isVisible) {
            //Number of Logs  
            int nbrLog = logs_.size();
            int nbrLogDisplayMax = ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing();

            //Tool Logger
            if (!ImGui::Begin("Logger Tool", &isVisible))
            {
                LimitationWindow();
                ImGui::End();
            }
            else
            {
                #pragma region Header
                //Removes all Logs
                if (ImGui::Button("Clear")) {
                    logs_.clear();
                }

                //scroll automatically
                ImGui::SameLine();
                if (ImGui::Checkbox("Auto Scrolling", &autoScroll)) {
                    autoScroll != autoScroll;
                }
                
                ImGui::SameLine();
                ImGui::Text(("Log Counter: " + std::to_string(nbrLog)).c_str());
                ImGui::Separator();
                #pragma endregion

                #pragma region Body
                ImGui::BeginChild("Logs");
                if (autoScroll) {
                    pos_y = nbrLog - nbrLogDisplayMax;
                    if (pos_y < 0) {
                        pos_y = 0;
                    }
                }
                else {
                    pos_y = ImGui::GetScrollY() / ImGui::GetTextLineHeightWithSpacing();
                    ImGui::SetCursorPos({ 0, (float)pos_y * ImGui::GetTextLineHeightWithSpacing() });
                }
              
                //Logs display
                if (nbrLog != 0) {
                    for (size_t i = pos_y; i < pos_y + (nbrLogDisplayMax); i++)
                    {
                        if (i < nbrLog) {
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
                    //Scroll Space
                    ImGui::SetCursorPos({ 0, (float)nbrLog * ImGui::GetTextLineHeightWithSpacing() });
                    ImGui::Text("");
                }
                ImGui::EndChild();
                LimitationWindow();
                ImGui::End();
                #pragma endregion 
            }
        }
      
    }

    void Logger::Log(LogSeverity severity, const std::string &msg)
    {
        if (logs_.size() < CAPACITY_LOG_MAX) {
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
        else {
            if (logs_.size() == CAPACITY_LOG_MAX) {
                // current date/time based on current system
                time_t now = time(0);
                // convert now to string form
                tm* ltm = localtime(&now);

                std::string time_log = "[" + std::to_string(ltm->tm_hour) + ":"
                    + std::to_string(ltm->tm_min) + ":"
                    + std::to_string(ltm->tm_sec) + "]";

                AerLog log;
                log.severity = LogSeverity::WARNING;
                log.msg = time_log + " [Warning] " + "[Logger] Max log capacity reached; Please Clear";

                logs_.emplace_back(log);
            }
        }
    }

    void Logger::OnEvent(const SDL_Event& event) { }
 

    void DebugLog(const std::string &msg) {   
        Log::get().Log(LogSeverity::DEBUG, msg);
    }

    void InfoLog(const std::string &msg) {
        Log::get().Log(LogSeverity::INFO, msg);
    }

    void WarningLog(const std::string &msg) {
        Log::get().Log(LogSeverity::WARNING, msg);
    }

    void ErrorLog(const std::string &msg) {
        Log::get().Log(LogSeverity::ERROOR, msg);
    }

    void CriticalLog(const std::string &msg) {
        Log::get().Log(LogSeverity::CRITICAL, msg);
    }
}
