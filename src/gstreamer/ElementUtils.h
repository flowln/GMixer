#pragma once

#include <gst/gst.h>
#include <list>
#include <memory>

struct plugin_info {
    const char* description;
    const char* version;
    const char* license;
};

struct pad_info {
    const char* caps;
    const char* availability;
};

struct element_info {
    const char* name;
    const char* description;
    const char* author;

    unsigned int num_srcs{ 0 };
    unsigned int num_sinks{ 0 };

    const std::list<pad_info>* src_pads;
    const std::list<pad_info>* sink_pads;
};

class ElementUtils {
   public:
    static const plugin_info* getPluginInfo(const std::string&);
    static element_info* getElementInfo(const std::string&);
    static const element_info* getElementInfo(const GstElement&);

   private:
};
