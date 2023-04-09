#pragma once

#include <exception>
#include <string>

/* Base class for logging facilities */
class Logger {
   public:
    /* Greater value = More restrict logging */
    enum class LoggingLevel { EVERYTHING = 0, DEBUG = 20, INFO = 40, WARNING = 60, ERROR = 80, NOTHING = 100 };

    Logger(LoggingLevel level = LoggingLevel::WARNING) : m_level(level){};
    virtual ~Logger() = default;

    virtual bool sendDebug(std::string&&) const;
    virtual bool sendInfo(std::string&&) const;
    virtual bool sendWarning(std::string&&) const;
    virtual bool sendError(std::string&&, std::exception* exception = nullptr) const;

   protected:
    LoggingLevel m_level;
};
