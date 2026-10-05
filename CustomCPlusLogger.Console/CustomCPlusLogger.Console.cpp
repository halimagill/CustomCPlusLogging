#include "ConfigReader.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"

#include <iostream>

int main()
{
    try
    {
        const ccpl::LoggerConfig config =
            ccpl::ConfigReader::load("logger.config");

        ccpl::ConsoleLogger consoleLogger(config);
        ccpl::FileLogger fileLogger(config);

        consoleLogger.info("Application started");
        fileLogger.info("Application started");

        consoleLogger.debug(
            "Testing debug logging",
            "Console debug test"
        );

        fileLogger.debug(
            "Testing debug logging",
            "File debug test"
        );

        consoleLogger.warning(
            "This is a warning",
            "Testing optional information"
        );

        fileLogger.warning(
            "This is a warning",
            "Testing optional information"
        );

        consoleLogger.error(
            "Something went wrong",
            "Test error details"
        );

        fileLogger.error(
            "Something went wrong",
            "Test error details"
        );

        consoleLogger.critical(
            "Critical test message"
        );

        fileLogger.critical(
            "Critical test message"
        );

        std::cout << "\nLogger test completed successfully.\n";
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Logger test failed: "
            << exception.what()
            << '\n';

        return 1;
    }

    return 0;
}