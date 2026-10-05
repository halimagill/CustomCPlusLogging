#include "pch.h"
#include "ConfigReader.h"

#include <fstream>
#include <stdexcept>
#include <string>

namespace ccpl
{
    // Reads logger configuration settings from a configuration file
    // and maps the supported values into a LoggerConfig object.
    //
    // The configuration file uses a simple key/value format:
    //
    //   IsConsoleLoggerEnabled = true
    //   IsFileLoggerEnabled = true
    //   Path = Logs
    //   FileName = application.log
    //   MaxFileSizeMB = 10
    //
    // ConfigReader also validates configuration values and reports
    // invalid or unsupported settings before the loggers are created.
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
            else if (key == "MaxFileSizeMB")
            {
                try
                {
                    config.maxFileSizeMB =
                        static_cast<std::size_t>(std::stoull(value));
                }
                catch (...)
                {
                    throw std::runtime_error(
                        "MaxFileSizeMB must be a valid positive whole number."
                    );
                }

                if (config.maxFileSizeMB == 0)
                {
                    throw std::runtime_error(
                        "MaxFileSizeMB must be greater than 0."
                    );
                }
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

    // Removes whitespace from the beginning and end of a configuration value.
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