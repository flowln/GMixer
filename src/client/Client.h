#pragma once

#include "client/common/Logger.h"

#include <memory>
#include <string>
#include <unordered_map>

class Client {
   public:
    Client();
    virtual ~Client() = default;

    virtual bool runClient(std::string&& dbus_name, int argc, char* argv[]) = 0;

    virtual std::shared_ptr<Logger> loggingAgent() = 0;

   protected:
    std::shared_ptr<Logger> m_logger;
};
