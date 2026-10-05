#pragma once

#include "logger.h"
#include "LoggerConfig.h"
#include <mutex>

namespace ccpl {
	class ConsoleLogger : public Logger
	{
	public:
		explicit ConsoleLogger(const LoggerConfig& config);

		void log(
			const std::string& message,
			LogLevel level,
			const std::string& optionalInfo = "", //Additional information that may need to be added for troubleshooting
			const std::source_location& location =
			std::source_location::current()
		) override;

	private:
		LoggerConfig config;
		std::mutex logMutex;
	};
}
