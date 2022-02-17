#include "client/cmd/CommandLineLogger.h"

CommandLineLogger::CommandLineLogger(Logger::LoggingLevel level) : Logger(level) {}

static const char* info_color    = "\033[34m";
static const char* warning_color = "\033[33m";
static const char* error_color   = "\033[31m";

bool CommandLineLogger::sendInfo(std::string&& message) const
{
    if (!Logger::sendInfo(""))
        return false;

    printf("%s[INFO] %s \033[0;39m\n", info_color, message.c_str());
    return true;
}

bool CommandLineLogger::sendWarning(std::string&& message) const
{
    if (!Logger::sendWarning(""))
        return false;

    printf("%s[WARNING] %s\n", warning_color, message.c_str());
    return true;
}

bool CommandLineLogger::sendError(std::string&& message) const
{
    if (!Logger::sendError(""))
        return false;

    printf("%s[ERROR] %s\n", error_color, message.c_str());
    return true;
}
