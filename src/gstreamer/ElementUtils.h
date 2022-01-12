#pragma once

#include <glibmm/ustring.h>
#include <gst/gst.h>
#include <list>
#include <memory>

struct plugin_info{
    const gchar* description;
    const gchar* version;
    const gchar* license;
};

struct pad_info{
    const gchar* caps;
    const gchar* availability;
};

struct element_info{
    const gchar* name;
    const gchar* description;
    const gchar* author;

    guint num_srcs { 0 };
    guint num_sinks { 0 };

    const std::list<pad_info>* src_pads;
    const std::list<pad_info>* sink_pads;
};

class ElementUtils{
    public:
        static const plugin_info* getPluginInfo(const Glib::ustring&);
        static element_info* getElementInfo(const Glib::ustring&);
        static const element_info* getElementInfo(const GstElement&);

    private:
    
};
