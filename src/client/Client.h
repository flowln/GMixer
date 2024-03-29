#pragma once

#include "client/common/Logger.h"
#include "client/common/Storage.h"

#include <memory>
#include <string>
#include <unordered_map>

class Client {
    friend class Initializer;

   public:
    static Client* instance() { return s_instance; }
    static std::shared_ptr<Logger> logger() { return s_instance->loggingAgent(); }

    virtual ~Client() = default;

    virtual bool runClient(std::string&& dbus_name, int argc, char* argv[]) = 0;

    virtual std::shared_ptr<Logger> loggingAgent()  = 0;
    virtual PipelineStorage* pipelineStorageAgent() = 0;

   protected:
    Client();

    std::shared_ptr<Logger> m_logger;

    static Client* s_instance;
};
