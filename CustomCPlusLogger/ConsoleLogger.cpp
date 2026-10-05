#include "pch.h"
#include "ConsoleLogger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace ccpl 
{
    ConsoleLogger::ConsoleLogger(const LoggerConfig& config)
        : config(config)
    {
    }
	void ConsoleLogger::log(
		const std::string& message,
		LogLevel level,
		const std::string& optionalInfo, //Additional information that may need to be added for troubleshooting
		const std::source_location& location) 
    {
        if (!config.isConsoleLoggerEnabled)
        {
            return;
        }

		std::lock_guard <std::mutex> lock(logMutex);
		const auto now = std::chrono::system_clock::now();
		const std::time_t time = std::chrono::system_clock::to_time_t(now);

		std::tm localTime{};

#ifdef _WIN32
		localtime_s(&localTime, &time);
#else
		localtime_r(&time, &localTime);
#endif

        std::cout
            << "["
            << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S")
            << "]"
            << " [" << logLevelToString(level) << "] "
            << message
            << " ["
            << location.file_name()
            << ":"
            << location.function_name()
            << ":"
            << location.line()
            << ":"
            << location.column()
            << "]";

        if (!optionalInfo.empty())
        {
            std::cout
                << " ["
                << optionalInfo
                << "]";
        }

        std::cout << '\n';
    }

}
