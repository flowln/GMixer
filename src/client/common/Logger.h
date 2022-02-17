#pragma once

#include <string>

/* Base class for logging facilities */
class Logger {
   public:
    /* Greater value = More restrict logging */
    enum class LoggingLevel { EVERYTHING = 0, INFO = 10, WARNING = 20, ERROR = 30, NOTHING = 40 };

    Logger(LoggingLevel level = LoggingLevel::WARNING) : m_level(level){};
    virtual ~Logger() = default;

    virtual bool sendInfo(std::string&&) const;
    virtual bool sendWarning(std::string&&) const;
    virtual bool sendError(std::string&&) const;

   protected:
    LoggingLevel m_level;
};
