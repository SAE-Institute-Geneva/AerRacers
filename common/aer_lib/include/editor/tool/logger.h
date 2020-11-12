#pragma once
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
 
 Author : Canas Simon
 Co-Author : Dylan von Arx
 Date : 13.10.2020
---------------------------------------------------------- */
#include <mutex>
#include <SDL_events.h>
#include <vector>

#include "editor/editor_tool_interface.h"

namespace neko::aer {
enum class LogSeverity : std::uint8_t {
    NONE = 0,
    DEBUG,
    INFO,
    WARNING,
    ERROOR,
    CRITICAL
};

struct AerLog {
    std::string msg;
    LogSeverity severity = LogSeverity::NONE;
};

//-----------------------------------------------------------------------------
// LogManagerInterface
//-----------------------------------------------------------------------------
class LogManagerInterface
{
public:
    virtual ~LogManagerInterface() = default;
    /**
     * \brief Generate a log message.
     * @param severity the type of the log message
     * @param msg the log message
     */
    virtual void Log(LogSeverity severity, const std::string& msg) = 0;

    /**
     * \brief Retrieves the logs
     */
    virtual const std::vector<AerLog>& GetLogs() = 0;

    /**
     * \brief Clear the logs
     */
    virtual void ClearLogs() = 0;
};

//-----------------------------------------------------------------------------
// NullLogManager
//-----------------------------------------------------------------------------
class NullLogManager final : public LogManagerInterface {
    void Log(
        [[maybe_unused]] LogSeverity severity,
        [[maybe_unused]] const std::string& msg) override
    {
        std::cout << msg;
        std::cout <<
            "[WARNING] LogManager is null! History will NOT be saved\n";
    }

    const std::vector<AerLog>& GetLogs() override
    {
        std::cerr << "Impossible to get log history from a null LogManager\n";
        assert(false);
#ifdef _MSC_VER
        return {};
#endif
    }

    void ClearLogs() override
    {
        std::cerr << "Impossible to clear log history from a null LogManager\n";
        assert(false);
    }
};

class Logger final : public EditorToolInterface, public LogManagerInterface {
public:
    explicit Logger(ToolType type, int id, std::string name);

    const std::vector<AerLog>& GetLogs() override
    {
        return logs_;
    }

    void Init() override;

    void Update(seconds dt) override;

    void DrawImGui() override;

    void Destroy() override;

    void OnEvent(const SDL_Event& event) override;

    /**
    * \brief Generate a log message.
    * @param severity the type of the log message
    * @param msg the log message
    */
    void Log(LogSeverity severity, const std::string& msg) override;
    /**
    * \brief Deletes all logs 
    */
    void ClearLogs() override;
    /**
   * \brief Exports the logs in a txt format file.
   */
    void WriteToFile();

private:
    int posY_ = 0;
    bool autoScroll_ = true;

    std::mutex logMutex_;
    std::vector<AerLog> logs_;

    //CAPACITY
    const size_t kCapacityLog_ = 1 << 14;
    const size_t kCapacityLogMax_ = 1 << 20;
    //COLOR
    const ImVec4 kBlue_ = {0.5f, 0.5f, 1, 1};
    const ImVec4 kYellow_ = {1, 1, 0, 1};
    const ImVec4 kOrange_ = {1, 0.5f, 0, 1};
    const ImVec4 kRed_{1, 0, 0, 1};
};

//-----------------------------------------------------------------------------
// Service Locator definition
//-----------------------------------------------------------------------------
using Log = Locator<LogManagerInterface, NullLogManager>;

void DebugLog(const std::string& msg);

void InfoLog(const std::string& msg);

void WarningLog(const std::string& msg);

void ErrorLog(const std::string& msg);

void CriticalLog(const std::string& msg);
}
