#pragma once

#include "client//Client.h"
#include "utils/FileUtils.h"

#include <glibmm/ustring.h>

class PipelineFactory {
   public:
    static void setClient(Client* client) { s_client = client; }
    static PipelineFactory* create();

    void createEmptyPipeline(std::string const& name);
    void createPipelineFromData(std::shared_ptr<FileUtils::file_info> data);

   private:
    static Client* s_client;

    PipelineFactory();
    std::shared_ptr<PipelineStorage> m_pipelines;
};
