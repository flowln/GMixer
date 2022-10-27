#pragma once

#include "client//Client.h"
#include "utils/FileUtils.h"

class PipelineFactory {
   public:
    static PipelineFactory* create();

    void createEmptyPipeline(std::string const& name);
    void createPipelineFromData(std::shared_ptr<FileUtils::file_info> data);

   private:
    PipelineFactory();
    std::shared_ptr<PipelineStorage> m_pipelines;
};
