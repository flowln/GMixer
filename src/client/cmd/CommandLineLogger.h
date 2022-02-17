#pragma once

#include "client/common/Logger.h"

class CommandLineLogger : public Logger {
   public:
    CommandLineLogger(LoggingLevel level = LoggingLevel::WARNING);

    virtual bool sendInfo(std::string&& message) const override;
    virtual bool sendWarning(std::string&& message) const override;
    virtual bool sendError(std::string&& message) const override;
};
