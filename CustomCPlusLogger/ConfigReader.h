#pragma once

#include <string>

#include "LoggerConfig.h"

namespace ccpl
{
	class ConfigReader
	{
	public:
		static LoggerConfig load(const std::string& filePath);
	
	private:
		static std::string trim(const std::string& value);
	};
}
