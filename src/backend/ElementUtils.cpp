#include "ElementUtils.h"
#include "gst/gstcompat.h"
#include "gst/gstelementfactory.h"
#include "gst/gstregistry.h"
#include <gst/gst.h>

const plugin_info* ElementUtils::getPluginInfo(const gchar* name)
{
    auto plugin = gst_plugin_load_by_name(name);
    if(!plugin)
        return nullptr;
    
    auto res = new plugin_info();

    res->name = name;
    res->description = gst_plugin_get_description(plugin);
    res->version = gst_plugin_get_version(plugin);
    res->license = gst_plugin_get_license(plugin);

    g_object_unref(plugin);

    return res;
}

const element_info* ElementUtils::getElementInfo(const gchar* name)
{
    auto factory = gst_element_factory_find(name);

    auto res = new element_info();

    res->name = name;
    res->description = gst_element_factory_get_description(factory);

    return res;
}
