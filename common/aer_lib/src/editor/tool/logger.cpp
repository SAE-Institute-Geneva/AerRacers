#include <utilities/file_utility.h>

#include "editor/tool/logger.h"

namespace neko::aer
{
Logger::Logger(TypeTool type) : Tool(type)
{
    Log::provide(this);
    logs_.reserve(kCapacityLog_);
}

void Logger::Init() { }

void Logger::Update(seconds dt) { }

void Logger::Destroy()
{
    WriteToFile();
    logs_.clear();
}


void Logger::DrawImGui()
{
    if (isVisible) {
        //Number of Logs  
        int nbrLog = logs_.size();
        const int nbrLogDisplayMax =
            ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing();

        //Tool Logger
        if (!ImGui::Begin("Logger Tool", &isVisible)) {
            LimitationWindow();
            ImGui::End();
        }
        else {
#pragma region Header
            //Removes all Logs
            if (ImGui::Button("Clear")) {
                ClearLogs();
                nbrLog = 0;
            }

            //Scroll automatically
            ImGui::SameLine();
            if (ImGui::Checkbox("Auto Scrolling", &autoScroll_)) {}

            ImGui::SameLine();
            ImGui::Text(("Log Counter: " + std::to_string(nbrLog)).c_str());
            if (nbrLog >= kCapacityLogMax_) {
                ImGui::SameLine();
                ImGui::TextColored(kRed_, "MAX");
            }
            ImGui::Separator();
#pragma endregion

#pragma region Body
            ImGui::BeginChild("Logs");
            if (autoScroll_) {
                posY_ = nbrLog - nbrLogDisplayMax;
                if (posY_ < 0) { posY_ = 0; }
            }
            else {
                posY_ = ImGui::GetScrollY() /
                        ImGui::GetTextLineHeightWithSpacing();
                ImGui::SetCursorPos(
                    {0, posY_ * ImGui::GetTextLineHeightWithSpacing()});
            }

            //Logs display
            if (nbrLog != 0) {
                for (size_t i = posY_; i < posY_ + nbrLogDisplayMax; i++) {
                    if (i < nbrLog) {
                        AerLog log = logs_[i];
                        switch (log.severity) {
                            case LogSeverity::DEBUG:
                                ImGui::TextColored(kBlue_, log.msg.c_str());
                                break;
                            case LogSeverity::INFO:
                                ImGui::Text(log.msg.c_str());
                                break;
                            case LogSeverity::WARNING:
                                ImGui::TextColored(kYellow_, log.msg.c_str());
                                break;
                            case LogSeverity::ERROOR:
                                ImGui::TextColored(kOrange_, log.msg.c_str());
                                break;
                            case LogSeverity::CRITICAL:
                                ImGui::TextColored(kRed_, log.msg.c_str());
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            else { ImGui::SetScrollY(0); }

            if (!autoScroll_) {
                //Scroll Space
                ImGui::SetCursorPos(
                    {0, nbrLog * ImGui::GetTextLineHeightWithSpacing()});
                ImGui::Text("");
            }
            ImGui::EndChild();
            LimitationWindow();
            ImGui::End();
#pragma endregion
        }
    }
}

void Logger::WriteToFile()
{
    std::lock_guard<std::mutex> lock(logMutex_);

    time_t curTime = time(nullptr);
#ifdef _MSC_VER
    struct tm localTime{};
    localtime_s(&localTime, &curTime);
#else
                tm localTime = *localtime(&curTime);
#endif

    const std::string filePath = "../../data/logs/";
    std::string dateTime;
    dateTime = std::to_string(localTime.tm_mday) + "-" + std::to_string(
                   localTime.tm_mon + 1) + "-" +
               std::to_string(localTime.tm_year + 1900) + "_" +
               std::to_string(localTime.tm_hour) + "-" +
               std::to_string(localTime.tm_min) + "-" +
               std::to_string(localTime.tm_sec);

    std::string fileContent =
        "/--------------------------------------------------------------------------------\\\n";
    fileContent +=
        "|                                NekoEngine logs                                 |\n";
    fileContent += "|                              " + dateTime +
        "                               |\n";
    fileContent +=
        "|              Copyright (c) 2020 SAE Institute Switzerland AG              |\n";
    fileContent +=
        "\\--------------------------------------------------------------------------------/\n\n";

    fileContent += "Program start (=^ O ^=)\n";
    fileContent +=
        "--------------------------------------------------------------------------------\n";

    AerLog log;
    log.severity = LogSeverity::DEBUG;
    log.msg = "Successfully saved log output";
    logs_.emplace_back(log);

    for (auto& line : logs_) { fileContent += line.msg + "\n"; }

    CreateDirectory(filePath);

    if (FileExists(filePath + dateTime + ".log"))
        dateTime += "-1";

    WriteStringToFile(filePath + dateTime + ".log", fileContent);
}


void Logger::Log(const LogSeverity severity, const std::string& msg)
{
    if (logs_.size() < kCapacityLogMax_) {
        // current date/time based on current system
        time_t now = time(0);
        // convert now to string form
        tm* ltm = localtime(&now);

        std::string time_log = "[" + std::to_string(ltm->tm_hour) + ":"
                               + std::to_string(ltm->tm_min) + ":"
                               + std::to_string(ltm->tm_sec) + "]";

        AerLog log;
        log.severity = severity;

        switch (log.severity) {
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
        if (logs_.size() == kCapacityLogMax_) {
            // current date/time based on current system
            time_t now = time(0);
            // convert now to string form
            tm* ltm = localtime(&now);

            std::string time_log = "[" + std::to_string(ltm->tm_hour) + ":"
                                   + std::to_string(ltm->tm_min) + ":"
                                   + std::to_string(ltm->tm_sec) + "]";

            AerLog log;
            log.severity = LogSeverity::WARNING;
            log.msg = time_log + " [Warning] " +
                      "[Logger] Max log capacity reached; Please Clear";

            logs_.emplace_back(log);
        }
    }
}

void Logger::ClearLogs() { logs_.clear(); }

void Logger::OnEvent(const SDL_Event& event) { }


void DebugLog(const std::string& msg)
{
    Log::get().Log(LogSeverity::DEBUG, msg);
}

void InfoLog(const std::string& msg) { Log::get().Log(LogSeverity::INFO, msg); }

void WarningLog(const std::string& msg)
{
    Log::get().Log(LogSeverity::WARNING, msg);
}

void ErrorLog(const std::string& msg)
{
    Log::get().Log(LogSeverity::ERROOR, msg);
}

void CriticalLog(const std::string& msg)
{
    Log::get().Log(LogSeverity::CRITICAL, msg);
}
}
