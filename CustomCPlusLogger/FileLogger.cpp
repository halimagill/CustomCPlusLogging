#include "pch.h"
#include "FileLogger.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <stdexcept>
#include <sstream>

namespace ccpl
{
    /// <summary>
    /// log()
    ///-> rolloverIfNeeded()
    ///    ->openLogFile() when needed
    ///    ->write
    ///    ->flush
    ///    ->verify write
    ///
    ///    openLogFile()
    ///    ->validate config
    ///    ->create directory
    ///    ->find correct daily / rollover file
    ///    ->open it
    ///
    ///    rolloverIfNeeded()
    ///    ->detect new day
    ///    ->detect closed file
    ///    ->detect size limit
    /// </summary>
    /// <param name="config"></param>
    FileLogger::FileLogger(const LoggerConfig& config)
        : config(config)
    {
    }    
    //Public Methods
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

        // Prevent multiple threads from writing to the console
        // at the same time and mixing their log entries together.
        std::lock_guard<std::mutex> lock(logMutex);

        rolloverIfNeeded();
                
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
        // Write the timestamp, log level, message, and source location
        // to the console.
        //
        // Example:
        // [2026-10-05 13:42:10] [ERROR] Something went wrong
        // [Main.cpp:int main():25:10]
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

       // Add optional troubleshooting or contextual information
       // only when it was supplied by the caller.
        if (!optionalInfo.empty())
        {
            logFile
                << " ["
                << optionalInfo
                << "]";
        }

        // Complete the log entry and move the console output
        // to the next line.
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

    //Private Methods
    
    // Builds the full path and file name for a daily log file.
    // The configured file name is separated into its base name and extension
    // so the date and rollover number can be added without changing the extension.
    //
    // Examples:
    //   application.log -> application_2026-10-05.log
    //   application.log -> application_2026-10-05_1.log
    //   application.log -> application_2026-10-05_2.log
    //
    // A rolloverIndex of 0 represents the first log file for the day.
    // Values greater than 0 are appended when the previous file reaches
    // the configured maximum file size.
    std::filesystem::path FileLogger::buildFilePath(
        const std::string& date,
        int rolloverIndex) const
    {
        const std::filesystem::path original(config.fileName);

        const std::string stem =
            original.stem().string();

        const std::string extension =
            original.extension().string();

        std::string generatedName =
            stem + "_" + date;

        if (rolloverIndex > 0)
        {
            generatedName +=
                "_" + std::to_string(rolloverIndex);
        }

        generatedName += extension;

        return std::filesystem::path(config.path)
            / generatedName;
    }

    //This gets the current date of the system, which will be used when creating the logging file name.
    std::string FileLogger::getCurrentDate() const
    {
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

        std::ostringstream date;

        date << std::put_time(
            &localTime,
            "%Y-%m-%d"
        );

        return date.str();
    }        

    std::uintmax_t FileLogger::getMaxFileSizeBytes() const
    {
        return static_cast<std::uintmax_t>(
            config.maxFileSizeMB
            ) * 1024 * 1024;
    }

    //finds or creates the correct file for the day.
    void FileLogger::openLogFile()
    {
        if (!config.isFileLoggerEnabled)
        {
            return;
        }

        if (logFile.is_open())
        {
            return;
        }

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

        if (config.maxFileSizeMB == 0)
        {
            throw std::runtime_error(
                "MaxFileSizeMB must be greater than 0."
            );
        }

        const std::filesystem::path directory(
            config.path
        );

        std::error_code error;

        if (!std::filesystem::exists(directory))
        {
            std::filesystem::create_directories(
                directory,
                error
            );

            if (error)
            {
                throw std::runtime_error(
                    "Unable to create log directory '" +
                    directory.string() +
                    "'. Error: " +
                    error.message()
                );
            }
        }

        currentDate =
            getCurrentDate();

        const std::uintmax_t maxFileSizeBytes = getMaxFileSizeBytes();

        int rolloverIndex = 0;

        std::filesystem::path candidate =
            buildFilePath(currentDate, rolloverIndex);

        while (std::filesystem::exists(candidate))
        {
            error.clear();

            const std::uintmax_t fileSize =
                std::filesystem::file_size(
                    candidate,
                    error
                );

            if (error)
            {
                throw std::runtime_error(
                    "Unable to determine log file size for '" +
                    candidate.string() +
                    "'. Error: " +
                    error.message()
                );
            }

            // We found an existing file that still has room.
            if (fileSize < maxFileSizeBytes)
            {
                break;
            }

            // Current file is full, try the next rollover number.
            ++rolloverIndex;

            candidate =
                buildFilePath(
                    currentDate,
                    rolloverIndex
                );
        }

        currentFilePath = candidate;

        logFile.open(
            currentFilePath,
            std::ios::out | std::ios::app
        );

        if (std::filesystem::exists(directory) &&
            !std::filesystem::is_directory(directory))
        {
            throw std::runtime_error(
                "The configured log path is not a directory: " +
                directory.string()
            );
        }
    }

    // decides whether the current file is still usable.
    void FileLogger::rolloverIfNeeded()
    {
        const std::string today = getCurrentDate();

        // New day
        if (currentDate != today)
        {
            if (logFile.is_open())
            {
                logFile.close();
            }

            currentDate.clear();
            currentFilePath.clear();

            openLogFile();

            return;
        }

        // First log or file was closed
        if (!logFile.is_open())
        {
            openLogFile();
            return;
        }

        std::error_code error;

        const std::uintmax_t fileSize =
            std::filesystem::file_size(
                currentFilePath,
                error
            );

        if (error)
        {
            throw std::runtime_error(
                "Unable to determine log file size for '" +
                currentFilePath.string() +
                "'. Error: " +
                error.message()
            );
        }

        const std::uintmax_t maxFileSizeBytes = getMaxFileSizeBytes();

        // Current file still has room.
        if (fileSize < maxFileSizeBytes)
        {
            return;
        }

        // Current file is full.
        logFile.close();
        currentFilePath.clear();

        openLogFile();
    }
}