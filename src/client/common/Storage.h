#pragma once

#include "gstreamer/Pipeline.h"

namespace {
static const std::string DEFAULT_ELEMENT_NAME = "ELEMENT NAME";
static const std::string DEFAULT_ELEMENT_PLUGIN_NAME = "PLUGIN NAME";
static const std::string DEFAULT_ELEMENT_PACKAGE = "PACKAGE NAME";
}

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
    [[nodiscard]] virtual std::string const& getElementName(unsigned int) const { return DEFAULT_ELEMENT_NAME; };
    [[nodiscard]] virtual std::string const& getElementPlugin(unsigned int) const { return DEFAULT_ELEMENT_PLUGIN_NAME; };
    [[nodiscard]] virtual std::string const& getElementPackage(unsigned int) const { return DEFAULT_ELEMENT_PACKAGE; };
};
