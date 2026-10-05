#include "ConfigReader.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"

#include <iostream>
#include <stdexcept>
#include <string>

int divide(int numerator, int denominator)
{
    if (denominator == 0)
    {
        throw std::runtime_error(
            "Cannot divide by zero."
        );
    }

    return numerator / denominator;
}

int main()
{
    try
    {
        const ccpl::LoggerConfig config =
            ccpl::ConfigReader::load("logger.config");

        ccpl::ConsoleLogger consoleLogger(config);
        ccpl::FileLogger fileLogger(config);

        consoleLogger.info("Logger test application started.");
        fileLogger.info("Logger test application started.");

        // Test optional information.
        consoleLogger.warning(
            "Testing optional troubleshooting information.",
            "This message contains additional context."
        );

        fileLogger.warning(
            "Testing optional troubleshooting information.",
            "This message contains additional context."
        );

        // Test exception handling and critical logging.
        try
        {
            const int result = divide(10, 0);

            std::cout << result << '\n';
        }
        catch (const std::exception& exception)
        {
            consoleLogger.critical(
                "A critical exception occurred.",
                exception.what()
            );

            fileLogger.critical(
                "A critical exception occurred.",
                exception.what()
            );
        }

        consoleLogger.info("Exception test completed.");
        fileLogger.info("Exception test completed.");

        // Test rollover functionality.
        //
        // Set MaxFileSizeMB = 1 in logger.config so the test
        // can force rollover without generating huge files.
        const std::string rolloverMessage(
            10000,
            'X'
        );

        for (int index = 1; index <= 500; ++index)
        {
            const std::string message =
                "Rollover test entry " +
                std::to_string(index);

            fileLogger.info(
                message,
                rolloverMessage
            );
        }

        consoleLogger.info(
            "Rollover test completed.",
            "Check the configured log directory for rollover files."
        );

        fileLogger.info(
            "Rollover test completed.",
            "The logger should have created multiple files."
        );

        consoleLogger.info("Logger test application completed.");
        fileLogger.info("Logger test application completed.");
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