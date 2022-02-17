#include "client/common/Logger.h"

bool Logger::sendInfo(std::string&&) const
{
    return m_level <= LoggingLevel::INFO;
}
bool Logger::sendWarning(std::string&&) const
{
    return m_level <= LoggingLevel::WARNING;
}
bool Logger::sendError(std::string&&) const
{
    return m_level <= LoggingLevel::ERROR;
}
