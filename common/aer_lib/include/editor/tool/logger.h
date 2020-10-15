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
 */
#include <vector>
#include <memory>
#include <SDL_events.h>
#include <editor/tool/tool.h>

namespace neko::aer
{
    enum class LogSeverity: std::uint8_t {
        NONE = 0,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    struct AerLog
    {
        std::string msg;
        LogSeverity severity;
    };
    //-----------------------------------------------------------------------------
// LogManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
    class LogManagerInterface
    {
    protected:
        ~LogManagerInterface() = default;
    public:
        /**
         * \brief Generate a log message.
         * @param logType the type of the log message
         * @param log the log message
         */
        virtual void Log(LogSeverity severity, const std::string msg) = 0;

        /**
         * \brief Retrieves the log history
         */
        virtual const std::vector<AerLog>& GetLogs() = 0;
    };

//-----------------------------------------------------------------------------
// NullLogManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
    class NullLogManager final : public LogManagerInterface
    {
        void Log([[maybe_unused]] LogSeverity severity,
            [[maybe_unused]] const std::string msg) override
        {
            std::cout << msg;
            std::cout << "[WARNING] LogManager is null! History will NOT be saved\n";
        }

        const std::vector<AerLog>& GetLogs() override
        {
            std::cerr << "Impossible to get log history from a null LogManager\n";
            assert(false);
#ifdef _MSC_VER
            return {};
#endif
        }
    };

    class Logger final : public Tool, public LogManagerInterface
    {
      
    public:
        Logger();
        
        const std::vector<AerLog>& GetLogs() override
        {
            return logs_;
        }

        /**
         * \brief Executed on the render thread
         */
        void Init() override;
        /**
         * \brief Executed on the main thread
         */
        void Update(seconds dt) override;
        /**
         * \brief Executed on the render thread
         */
        void DrawImGui() override;
        /**
         * \brief Executed on the render thread
         */
        void Destroy() override;
        /**
         * \brief Executed on the main thread
         */
        void OnEvent(const SDL_Event& event) override;

        void Log(LogSeverity severity, const std::string msg) override;
    private:
        int pos_y = 0;
        bool scrollToBottom = true;


        std::vector<AerLog> logs_;

       

    };

//-----------------------------------------------------------------------------
// Service Locator definition
//-----------------------------------------------------------------------------
    using Log = Locator<LogManagerInterface, NullLogManager>;

    void DebugLog(std::string msg);

    void InfoLog(std::string msg);

    void WarningLog(std::string msg);

    void ErrorLog(std::string msg);

    void CriticalLog(std::string msg);
}