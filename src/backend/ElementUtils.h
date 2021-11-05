#pragma once

#include <glibmm/ustring.h>
#include <glibmm/object.h>

struct plugin_info{
    const gchar* description;
    const gchar* version;
    const gchar* license;

    public:
        ~plugin_info(){
            delete[] description;
            delete[] version;
            delete[] license;
        }
};

struct element_info{
    const gchar* description;

    public:
        ~element_info(){
            delete[] description;
        }
};

class ElementUtils{
    public:
        static const plugin_info* getPluginInfo(const Glib::ustring&);
        static const element_info* getElementInfo(const Glib::ustring&);
};
