#include "pch.h"
#include <string>
#include "Logger.h"

namespace ccpl
{
    
    void Logger::info(
        const std::string& message,
        const std::string& optionalInfo,
        const std::source_location& location)
    {
        this->log(message, LogLevel::Info, optionalInfo, location);
    }

    void Logger::debug(
        const std::string& message,
        const std::string& optionalInfo,
        const std::source_location& location)
    {
        this->log(message, LogLevel::Debug, optionalInfo, location);
    }

    void Logger::warning(
        const std::string& message,
        const std::string& optionalInfo,
        const std::source_location& location)
    {
        this->log(message, LogLevel::Warning, optionalInfo, location);
    }

    void Logger::error(
        const std::string& message,
        const std::string& optionalInfo,
        const std::source_location& location)
    {
        this->log(message, LogLevel::Error, optionalInfo, location);
    }

    void Logger::critical(
        const std::string& message,
        const std::string& optionalInfo,
        const std::source_location& location)
    {
        this->log(message, LogLevel::Critical, optionalInfo, location);
    }

	std::string Logger::logLevelToString(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Debug:
			return "DEBUG";
		case LogLevel::Info:
			return "INFO";
		case LogLevel::Warning:
			return "WARNING";
		case LogLevel::Error:
			return "Error";
		case LogLevel::Critical:
			return "CRITICAL";
		default:
			return "UPEXPECTED";
		}
	}
}