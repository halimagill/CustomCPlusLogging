#include "pch.h"
#include "ConsoleLogger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace ccpl 
{
    // Stores a copy of the logger configuration so the console logger
    // can determine whether console logging is enabled.
    ConsoleLogger::ConsoleLogger(const LoggerConfig& config)
        : config(config)
    {
    }

    // Writes a formatted log entry to the console when console
    // logging is enabled.
	void ConsoleLogger::log(
		const std::string& message,
		LogLevel level,
		const std::string& optionalInfo, //Additional information that may need to be added for troubleshooting
		const std::source_location& location) 
    {
        // Console logging is disabled, so do nothing.
        if (!config.isConsoleLoggerEnabled)
        {
            return;
        }

        // Prevent multiple threads from writing to the console
        // at the same time and mixing their log entries together.
		std::lock_guard <std::mutex> lock(logMutex);
		const auto now = std::chrono::system_clock::now();
		const std::time_t time = std::chrono::system_clock::to_time_t(now);

		std::tm localTime{};

        // Convert the system time to local time using the
        // thread-safe function available for the current platform.
#ifdef _WIN32
		localtime_s(&localTime, &time);
#else
		localtime_r(&time, &localTime);
#endif

        // Write the timestamp, log level, message, and source location
        // to the console.
        //
        // Example:
        // [2026-10-05 13:42:10] [ERROR] Something went wrong
        // [Main.cpp:int main():25:10]
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

        // Add optional troubleshooting or contextual information
        // only when it was supplied by the caller.
        if (!optionalInfo.empty())
        {
            std::cout
                << " ["
                << optionalInfo
                << "]";
        }

        // Complete the log entry and move the console output
        // to the next line.
        std::cout << '\n';
    }

}
