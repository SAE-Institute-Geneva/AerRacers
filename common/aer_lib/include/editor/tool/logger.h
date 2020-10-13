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
    class Logger : public Tool
    {
      
    public:
        Logger() {
            logs_.reserve(100000);
        };
        
        struct MyLog
        {
            std::string log_msg;
            int log_severity;
        };

        static Logger* get() noexcept {
            return (!instance) ?
                instance = new Logger :
                instance;
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

        void AddLog(std::string msg_log, int severity_log);

        Logger(Logger const&) = delete;
        void operator=(Logger const&) = delete;

        static Logger* instance;
    protected:


        int scroll_Y = 0;
        bool scrollToBottom = true;


        std::vector<MyLog> logs_;
    private:
       

    };

    void DebugLog(std::string msg);

    void InfoLog(std::string msg);

    void WarningLog(std::string msg);

    void ErrorLog(std::string msg);

    void CriticalLog(std::string msg);
}