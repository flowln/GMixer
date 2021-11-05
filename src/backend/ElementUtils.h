#pragma once

#include <glibmm/ustring.h>
#include <glibmm/object.h>

struct plugin_info{
    const gchar* description;
    const gchar* version;
    const gchar* license;
};

struct element_info{
    const gchar* description;
};

class ElementUtils{
    public:
        static const plugin_info* getPluginInfo(const Glib::ustring&);
        static const element_info* getElementInfo(const Glib::ustring&);
};
