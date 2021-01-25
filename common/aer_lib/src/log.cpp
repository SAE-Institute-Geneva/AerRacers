#include "aer/log.h"

#include <iomanip>
#include <memory>
#include <sstream>
#ifdef EASY_PROFILE_USE
    #include "easy/profiler.h"
#endif

#include <utils/file_utility.h>

namespace neko
{
//-----------------------------------------------------------------------------
// LogMessage definitions
//-----------------------------------------------------------------------------
void LogMessage::Generate()
{
	time_t curTime = time(nullptr);

#ifdef _MSC_VER
	struct tm localTime
	{};
	localtime_s(&localTime, &curTime);
#else
	tm localTime = *localtime(&curTime);
#endif

	std::ostringstream message;
	message << '[' << std::setw(2) << std::setfill('0') << localTime.tm_hour << ":" << std::setw(2)
			<< std::setfill('0') << localTime.tm_min << ":" << std::setw(2) << std::setfill('0')
			<< localTime.tm_sec << "] ";

	switch (type)
	{
		case LogType::DEBUG_: message << "[DEBUG] "; break;
		case LogType::INFO: message << "[INFO] "; break;
		case LogType::WARNING: message << "[WARNING] "; break;
		case LogType::ERROR_: message << "[ERROR] "; break;
		case LogType::CRITICAL: message << "[CRITICAL] "; break;
		default: break;
	}

	switch (category)
	{
		case LogCategory::ENGINE: message << "[ENGINE] "; break;
		case LogCategory::MATH: message << "[MATH] "; break;
		case LogCategory::GRAPHICS: message << "[GRAPHICS] "; break;
		case LogCategory::IO: message << "[IO] "; break;
		case LogCategory::SOUND: message << "[SOUND] "; break;
		case LogCategory::TOOL: message << "[TOOL] "; break;
		default: break;
	}

	message << log << '\n';
	log = message.str();
}

//-----------------------------------------------------------------------------
// LogManager definitions
//-----------------------------------------------------------------------------
LogManager::LogManager() : status_(0)
{
	Log::provide(this);
	status_ |= IS_RUNNING | IS_EMPTY;
	logThread_ = std::make_unique<std::thread>([this] { LogLoop(); });
}

LogManager::~LogManager()
{
	//WriteToFile();
	Destroy();
}

void LogManager::LogLoop()
{
    do
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("LogManager::LogLoop");
#endif
		if (status_ & IS_EMPTY)
		{
			std::unique_lock<std::mutex> lock(logMutex_);

			status_ |= IS_LOG_WAITING;
			conditionVariable_.wait(lock);
			status_ &= ~IS_LOG_WAITING;
		}
		else
		{
			std::function<void()> task;
			{
				std::lock_guard<std::mutex> lock(logMutex_);
				task = tasks_.front();
				tasks_.erase(tasks_.begin());
				if (tasks_.empty()) status_ |= IS_EMPTY;
			}
			task();
			conditionVariable_.notify_one();
		}
	} while (status_ & IS_RUNNING);
}

void LogManager::Wait()
{
	while (!(status_ & IS_EMPTY))
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("LogManager::Wait");
#endif
		std::unique_lock<std::mutex> lock(logMutex_);
		if (tasks_.empty()) status_ |= IS_EMPTY;
		else
		{
			status_ |= IS_LOG_WAITING;
			conditionVariable_.wait(lock);
			status_ &= ~IS_LOG_WAITING;
		}
	}
}

void LogManager::Destroy()
{
	std::unique_lock<std::mutex> lock(logMutex_);
	if (status_ & IS_WRITING)
	{
		status_ |= IS_LOG_WAITING;
		conditionVariable_.wait(lock);
		status_ &= ~IS_LOG_WAITING;
	}

	{
		conditionVariable_.notify_one();

		status_ &= ~IS_RUNNING;
	}
	lock.unlock();

	logThread_->join();
}

void LogManager::Log(LogType logType, const std::string& log)
{
	Log(LogCategory::NONE, logType, log);
}

void LogManager::Log(LogCategory category, LogType logType, const std::string& log)
{
	std::lock_guard<std::mutex> lock(logMutex_);
	status_ &= ~IS_EMPTY;

	auto task([this, logType, category, log]
	{
		LogMessage message(category, logType, log);
		message.Display();

		logHistory_.emplace_back(message);
	});

	tasks_.emplace_back(task);
	conditionVariable_.notify_one();
}

void LogManager::ClearLogs() { logHistory_.clear(); }

void LogManager::WriteToFile()
{
	std::lock_guard<std::mutex> lock(logMutex_);

	status_ |= IS_WRITING;
	status_ &= ~IS_EMPTY;

	auto task([this]
	{
        time_t curTime = time(nullptr);
#ifdef _MSC_VER
        struct tm localTime {};
	    localtime_s(&localTime, &curTime);
#else
	    tm localTime = *localtime(&curTime);
#endif

		const std::string filePath = "../../data/logs/";
		std::string dateTime;
        dateTime = std::to_string(localTime.tm_mday) + "-" + std::to_string(localTime.tm_mon+1) + "-" +
                   std::to_string(localTime.tm_year + 1900) + "_" +
                   std::to_string(localTime.tm_hour) + "-" +
                   std::to_string(localTime.tm_min) + "-" +
                   std::to_string(localTime.tm_sec);

		std::string fileContent =
			"/--------------------------------------------------------------------------------\\\n";
		fileContent +=
			"|                                NekoEngine logs                                 |\n";
		fileContent += "|                              " + dateTime + "                               |\n";
		fileContent +=
			"|              Copyright (c) 2020 SAE Institute Switzerland AG              |\n";
		fileContent +=
			"\\--------------------------------------------------------------------------------/\n\n";

		fileContent += "Program start (=^ O ^=)\n";
		fileContent +=
			"--------------------------------------------------------------------------------\n";

		LogMessage message(LogCategory::IO, LogType::INFO, "Successfully saved log output");
		logHistory_.emplace_back(message);
		message.Display();

		for (auto& line : logHistory_)
		{
			fileContent += line.log;
		}

		CreateDirectory(filePath);

		//if (FileExists(filePath + dateTime + ".log"))
		//	dateTime += "-1";

		WriteStringToFile(filePath + dateTime + ".log", fileContent);

		status_ &= ~IS_WRITING;
	});

	tasks_.emplace_back(task);
	conditionVariable_.notify_one();
}

//-----------------------------------------------------------------------------
// Shorthands definitions
//-----------------------------------------------------------------------------
void LogDebug(const std::string& msg) { Log::get().Log(LogType::INFO, msg); }

void LogDebug(const LogCategory category, const std::string& msg)
{
	Log::get().Log(category, LogType::INFO, msg);
}

void LogInfo(const std::string& msg) { Log::get().Log(LogType::INFO, msg); }

void LogInfo(const LogCategory category, const std::string& msg)
{
	Log::get().Log(category, LogType::INFO, msg);
}

void LogWarning(const std::string& msg) { Log::get().Log(LogType::WARNING, msg); }

void LogWarning(const LogCategory category, const std::string& msg)
{
	Log::get().Log(category, LogType::WARNING, msg);
}

void LogError(const std::string& msg) { Log::get().Log(LogType::ERROR_, msg); }

void LogError(const LogCategory category, const std::string& msg)
{
	Log::get().Log(category, LogType::ERROR_, msg);
}

void LogCritical(const std::string& msg) { Log::get().Log(LogType::CRITICAL, msg); }

void LogCritical(const LogCategory category, const std::string& msg)
{
	Log::get().Log(category, LogType::CRITICAL, msg);
}

const std::vector<LogMessage>& GetLogs() { return Log::get().GetLogs(); }
}
