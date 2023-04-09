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

    virtual bool sendDebug(std::string&&) const
    {
        return m_level <= LoggingLevel::DEBUG;
    }
    virtual bool sendInfo(std::string&&) const
    {
        return m_level <= LoggingLevel::INFO;
    }
    virtual bool sendWarning(std::string&&) const
    {
        return m_level <= LoggingLevel::WARNING;
    }
    virtual bool sendError(std::string&&, std::exception* exception = nullptr) const
    {
        return m_level <= LoggingLevel::ERROR;
    }

   protected:
    LoggingLevel m_level;
};
