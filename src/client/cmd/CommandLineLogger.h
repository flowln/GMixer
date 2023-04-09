#pragma once

#include "client/common/Logger.h"

class CommandLineLogger : public Logger {
   public:
    CommandLineLogger(LoggingLevel level = LoggingLevel::WARNING);

    bool sendDebug(std::string&& message) const override;
    bool sendInfo(std::string&& message) const override;
    bool sendWarning(std::string&& message) const override;
    bool sendError(std::string&& message, std::exception* exc = nullptr) const override;
};
