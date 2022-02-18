#pragma once

#include "gstreamer/Pipeline.h"

class PipelineStorage {
   public:
    virtual ~PipelineStorage() = default;

    virtual unsigned int numEntries() const                   = 0;
    virtual std::weak_ptr<Pipeline> getPipeline(unsigned int) = 0;
    virtual void addPipeline(Pipeline*)                       = 0;
    virtual void addPipeline(std::string&&, Pipeline*)        = 0;
};
