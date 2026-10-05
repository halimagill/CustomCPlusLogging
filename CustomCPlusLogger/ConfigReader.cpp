#include "pch.h"
#include "ConfigReader.h"

#include <fstream>
#include <stdexcept>
#include <string>

namespace ccpl
{
    LoggerConfig ConfigReader::load(const std::string& filePath)
    {
        std::ifstream configFile(filePath);

        if (!configFile.is_open())
        {
            throw std::runtime_error(
                "Unable to open logger configuration file: " + filePath
            );
        }

        LoggerConfig config;
        std::string line;

        while (std::getline(configFile, line))
        {
            line = trim(line);

            // Ignore blank lines
            if (line.empty())
            {
                continue;
            }

            // Ignore comments
            if (line.starts_with('#'))
            {
                continue;
            }

            const std::size_t separatorPosition = line.find('=');

            if (separatorPosition == std::string::npos)
            {
                throw std::runtime_error(
                    "Invalid logger configuration line: " + line
                );
            }

            const std::string key =
                trim(line.substr(0, separatorPosition));

            const std::string value =
                trim(line.substr(separatorPosition + 1));

            if (key == "IsConsoleLoggerEnabled")
            {
                if (value == "true")
                {
                    config.isConsoleLoggerEnabled = true;
                }
                else if (value == "false")
                {
                    config.isConsoleLoggerEnabled = false;
                }
                else
                {
                    throw std::runtime_error(
                        "IsConsoleLoggerEnabled must be true or false."
                    );
                }
            }
            else if (key == "IsFileLoggerEnabled")
            {
                if (value == "true")
                {
                    config.isFileLoggerEnabled = true;
                }
                else if (value == "false")
                {
                    config.isFileLoggerEnabled = false;
                }
                else
                {
                    throw std::runtime_error(
                        "IsFileLoggerEnabled must be true or false."
                    );
                }
            }
            else if (key == "Path")
            {
                config.path = value;
            }
            else if (key == "FileName")
            {
                config.fileName = value;
            }
            else
            {
                throw std::runtime_error(
                    "Unknown logger configuration setting: " + key
                );
            }
        }

        if (config.isFileLoggerEnabled)
        {
            if (config.path.empty())
            {
                throw std::runtime_error(
                    "File logging is enabled, but Path is empty."
                );
            }

            if (config.fileName.empty())
            {
                throw std::runtime_error(
                    "File logging is enabled, but FileName is empty."
                );
            }
        }

        return config;
    }

    std::string ccpl::ConfigReader::trim(const std::string& value)
    {
        const std::string whitespace = " \t\r\n";

        const std::size_t start = value.find_first_not_of(whitespace);

        if (start == std::string::npos)
        {
            return "";
        }

        const std::size_t end = value.find_last_not_of(whitespace);

        return value.substr(start, end - start + 1);
    }
}