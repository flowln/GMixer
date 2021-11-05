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

struct element_info{
    const gchar* name;
    const gchar* description;

    public:
        ~element_info(){
            delete[] name;
            delete[] description;
        }
};

class ElementUtils{
    public:
        static const plugin_info* getPluginInfo(const gchar*);
        static const element_info* getElementInfo(const gchar*);
};
