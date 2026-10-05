#pragma once

#include "Logger.h"
#include "LoggerConfig.h"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace ccpl
{
    class FileLogger : public Logger
    {
    public:
        explicit FileLogger(const LoggerConfig& config);

        void log(
            const std::string& message,
            LogLevel level,
            const std::string& optionalInfo = "",
            const std::source_location& location =
            std::source_location::current()
        ) override;

    private:
        LoggerConfig config;
        std::ofstream logFile;
        std::mutex logMutex;

        std::string currentDate;
        std::filesystem::path currentFilePath;

        std::filesystem::path buildFilePath(const std::string& date, int rolloverIndex) const;
        std::string getCurrentDate() const;
        std::uintmax_t getMaxFileSizeBytes() const;
        void openLogFile();
        void rolloverIfNeeded();
    };
}