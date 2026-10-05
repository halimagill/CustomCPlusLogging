#include "pch.h"
#include "FileLogger.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <stdexcept>

namespace ccpl
{
    FileLogger::FileLogger(const LoggerConfig& config)
        : config(config)
    {
    }

    void FileLogger::openLogFile()
    {
        // If the file is already open, there is nothing to do.
        if (logFile.is_open())
        {
            return;
        }

        if (config.path.empty())
        {
            throw std::runtime_error(
                "File logging is enabled, but a log path "
                "has not been configured."
            );
        }

        if (config.fileName.empty())
        {
            throw std::runtime_error(
                "File logging is enabled, but a log file name "
                "has not been configured."
            );
        }

        const std::filesystem::path directory(config.path);

        std::error_code error;

        // Create the directory if it does not already exist.
        if (!std::filesystem::exists(directory))
        {
            std::filesystem::create_directories(
                directory,
                error
            );

            if (error)
            {
                throw std::runtime_error(
                    "Unable to create the log directory '" +
                    directory.string() +
                    "'. Error: " +
                    error.message()
                );
            }
        }

        const std::filesystem::path filePath =
            directory / config.fileName;

        // Open the existing file or create it if it does not exist.
        // app prevents existing logs from being overwritten.
        logFile.open(
            filePath,
            std::ios::out | std::ios::app
        );

        if (!logFile.is_open())
        {
            throw std::runtime_error(
                "Unable to create or open the log file '" +
                filePath.string() +
                "'. Verify that the application has permission "
                "to write to this location."
            );
        }
    }

    void FileLogger::log(
        const std::string& message,
        LogLevel level,
        const std::string& optionalInfo,
        const std::source_location& location)
    {
        // File logging is disabled, so do nothing.
        if (!config.isFileLoggerEnabled)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(logMutex);

        // The file isn't created until something actually needs
        // to be logged.
        openLogFile();

        const auto now =
            std::chrono::system_clock::now();

        const std::time_t time =
            std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};

#ifdef _WIN32
        localtime_s(&localTime, &time);
#else
        localtime_r(&time, &localTime);
#endif

        logFile
            << "["
            << std::put_time(
                &localTime,
                "%Y-%m-%d %H:%M:%S"
            )
            << "]"
            << " ["
            << logLevelToString(level)
            << "] "
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
            logFile
                << " ["
                << optionalInfo
                << "]";
        }

        logFile << '\n';

        // Write immediately rather than leaving the entry
        // sitting in the stream buffer.
        logFile.flush();

        if (logFile.fail())
        {
            throw std::runtime_error(
                "An error occurred while writing to the log file."
            );
        }
    }
}