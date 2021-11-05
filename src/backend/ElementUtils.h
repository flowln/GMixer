#pragma once

#include <glibmm/ustring.h>
#include <glibmm/object.h>

struct plugin_info{
    const gchar* name;
    const gchar* description;
    const gchar* version;
    const gchar* license;

    public:
        ~plugin_info(){
            delete[] name;
            delete[] description;
            delete[] version;
            delete[] license;
        }
};

class ElementUtils{
    public:
        static const plugin_info* getPluginInfo(const gchar*);
};
