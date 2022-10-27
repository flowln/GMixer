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

class ElementStorage {
   public:
    virtual ~ElementStorage() = default;

    virtual unsigned int numEntries() const      = 0;
    virtual void populate(bool blocking = false) = 0;

    // FIXME: Make a decent API for this storage
    virtual const std::string getElementName(unsigned int) const { return ""; };
    virtual const std::string getElementPlugin(unsigned int) const { return ""; };
    virtual const std::string getElementPackage(unsigned int) const { return ""; };
};
