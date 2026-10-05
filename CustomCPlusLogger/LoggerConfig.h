#pragma once

#include <string>

namespace ccpl
{
    struct LoggerConfig
    {
        bool isConsoleLoggerEnabled = true;
        bool isFileLoggerEnabled = false;
        std::string path;
        std::string fileName;
    };
}