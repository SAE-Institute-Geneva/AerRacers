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
#include <vector>
#include <memory> //TODOCR(Luca@Dylan) Unused include 
#include <mutex>
#include <math.h> //TODOCR(Luca@Dylan) Unused include 

#include <SDL_events.h>

#include "editor/tool.h"

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
    LogSeverity severity;
};

//-----------------------------------------------------------------------------
// LogManagerInterface
//-----------------------------------------------------------------------------
class LogManagerInterface {
protected:
    ~LogManagerInterface() = default;
public:
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

class Logger final : public Tool, public LogManagerInterface {
    //-----------------------------------------------------------------------------
    // LogManagerStatus
    //-----------------------------------------------------------------------------
    /// \brief To get the status of the engine
    enum LogManagerStatus : std::uint8_t { //TODOCR(Luca@Dylan) Unused Enum
        IS_RUNNING = 1u << 0u,
        //To check if the LogManager is running
        IS_EMPTY = 1u << 1u,
        //To check if the LogManager has tasks
        IS_LOG_WAITING = 1u << 2u,
        //To check if the LogManager is waiting for a task
        IS_WRITING = 1u << 3u
        //To check if the LogManager is writing its output to a file
    };

public:
    //Constructor
    Logger(TypeTool type);
    // conversion from Tool (assignment):
    Logger* operator=(const Tool* tool) { return this; }

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

    void WriteToFile();

private:
    int pos_y = 0; //TODOCR(Luca@Dylan) Nomenclature 
    bool autoScroll = true; //TODOCR(Luca@Dylan) Nomenclature 

    std::atomic<std::uint8_t> status_;
    std::mutex logMutex_;
    std::vector<AerLog> logs_;

    //CAPACITY
    const size_t kCapacityLog_ = std::pow(2, 14);
    const size_t kCapacityLogMax_ = std::pow(2, 20);
    //COLOR
    const ImVec4 kBlue_ = {0.5f, 0.5f, 1, 1}; //TODOCR(Luca@Dylan) Nomenclature 
    const ImVec4 YELLOW = {1, 1, 0, 1}; //TODOCR(Luca@Dylan) Nomenclature 
    const ImVec4 ORANGE = {1, 0.5f, 0, 1}; //TODOCR(Luca@Dylan) Nomenclature 
    const ImVec4 RED{1, 0, 0, 1}; //TODOCR(Luca@Dylan) Nomenclature 
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
