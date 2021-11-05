#include "ElementListModel.h"
#include "gst/gstelementfactory.h"

#include <iostream>

#include <gst/gst.h>

ElementRecord::ElementRecord()
{
    add(m_element_name);
    add(m_plugin);
    add(m_package);
}

gboolean checkFilter(GstElementFactory* factory)
{
    gboolean result;
    auto e = gst_element_factory_create(factory, "");
    result = (e->numsinkpads > 0) && (e->numsrcpads > 0);
    g_object_unref(e);
    return result;
}

gboolean checkFeatureType(GstPluginFeature* feature, ElementType& type){
    auto f = gst_element_factory_find(gst_plugin_feature_get_name(feature));

    if(!f)
        return false;
    
    bool result = false;
    switch(type){
        case ElementType::SOURCE:
            result = gst_element_factory_list_is_type(f, GST_ELEMENT_FACTORY_TYPE_SRC);
            break;
        case ElementType::FILTER:
            //FIXME: How to do this without creating a new element instance?
            result = checkFilter(f);

            break;
        case ElementType::SINK:
            result = gst_element_factory_list_is_type(f, GST_ELEMENT_FACTORY_TYPE_SINK);
            break;
        default:
            break;
    }
    g_object_unref(f); 
    return result;
}

ElementListModel::ElementListModel(ElementType type)
    : m_type(type)
{
    m_store = Gtk::ListStore::create(m_records);
}

/* Heavily inspired by the gst-inspect source code
 * https://github.com/GStreamer/gstreamer/blob/master/tools/gst-inspect.c */
void ElementListModel::populate()
{
    if(isPopulated)
        return;

    auto registry = gst_registry_get();

    GList* plugins = gst_registry_get_plugin_list(registry);
    while(plugins){
        auto plugin = GST_PLUGIN(plugins->data);
        plugins = g_list_next(plugins);

        if (GST_OBJECT_FLAG_IS_SET (plugin, GST_PLUGIN_FLAG_BLACKLISTED))
            continue;

        GList* features = gst_registry_get_feature_list_by_plugin(registry, gst_plugin_get_name(plugin));
        while(features){
            auto feature = GST_PLUGIN_FEATURE(features->data);
            features = g_list_next(features);
            if(!checkFeatureType(feature, m_type))
                continue;

            addElement(plugin, feature);
        }
        gst_plugin_feature_list_free(features);

    }
    gst_plugin_list_free(plugins);
    isPopulated = true;
}

void ElementListModel::addElement(GstPlugin* plugin, GstPluginFeature* element)
{
    auto iter = m_store->prepend();

    (*iter)[m_records.m_element_name] = gst_element_get_name(element); 
    (*iter)[m_records.m_plugin] = gst_plugin_get_name(plugin); 
    (*iter)[m_records.m_package] = gst_plugin_get_package(plugin);
}
