#include "client/cmd/CommandLineLogger.h"
#include <exception>

CommandLineLogger::CommandLineLogger(Logger::LoggingLevel level) : Logger(level) {}

static const char* end_message   = "\033[0;39m\n";
static const char* info_color    = "\033[34m";
static const char* warning_color = "\033[33m";
static const char* error_color   = "\033[31m";

bool CommandLineLogger::sendInfo(std::string&& message) const
{
    if (!Logger::sendInfo(""))
        return false;

    printf("%s[INFO] %s %s", info_color, message.c_str(), end_message);
    return true;
}

bool CommandLineLogger::sendWarning(std::string&& message) const
{
    if (!Logger::sendWarning(""))
        return false;

    printf("%s[WARNING] %s %s", warning_color, message.c_str(), end_message);
    return true;
}

bool CommandLineLogger::sendError(std::string&& message, std::exception* exc) const
{
    if (!Logger::sendError(""))
        return false;

    if (exc) {
        printf("%s[ERROR] %s:\n\t--> %s %s", error_color, message.c_str(), exc->what(), end_message);
    } else {
        printf("%s[ERROR] %s %s", error_color, message.c_str(), end_message);
    }
    return true;
}
