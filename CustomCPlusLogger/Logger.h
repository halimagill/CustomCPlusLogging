#pragma once

#include <source_location>
#include <string>

#include "LogLevel.h"

namespace ccpl
{
	class Logger
	{
	public:
		virtual ~Logger() = default;

		virtual void log(
			const std::string& message,
			LogLevel level,
            const std::string& optionalInfo = "", //Additional information that may need to be added for troubleshooting
			const std::source_location& location =
				std::source_location::current()			
		) = 0;

        void info(
            const std::string& message,
            const std::string& optionalInfo = "",
            const std::source_location& location =
            std::source_location::current());

        void debug(
            const std::string& message,
            const std::string& optionalInfo = "",
            const std::source_location& location =
            std::source_location::current());

        void warning(
            const std::string& message,
            const std::string& optionalInfo = "",
            const std::source_location& location =
            std::source_location::current());

        void error(
            const std::string& message,
            const std::string& optionalInfo = "",
            const std::source_location& location =
            std::source_location::current());

        void critical(
            const std::string& message,
            const std::string& optionalInfo = "",
            const std::source_location& location =
            std::source_location::current());

	protected:
		static std::string logLevelToString(LogLevel level);
	};
}
