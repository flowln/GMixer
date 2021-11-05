#include "ElementListModel.h"

#include <gst/gst.h>

#include <iostream>

ElementRecord::ElementRecord()
{
    add(m_element_name);
    add(m_plugin);
    add(m_package);
}

gboolean checkFeatureType(GstPluginFeature* feature, ElementType& type){
    auto e = gst_element_factory_make(gst_plugin_feature_get_name(feature), "");
    if(!e) 
        return false;

    if(e->numpads == 0)
        return false;
    
    bool result = false;
    switch(type){
        case ElementType::SOURCE:
            result = (e->numsinkpads == 0);
            break;
        case ElementType::FILTER:
            result = ((e->numsinkpads != 0) && (e->numsrcpads != 0));
            break;
        case ElementType::SINK:
            result = (e->numsrcpads == 0);
            break;
        default:
            break;
    }
    g_object_unref(e); 
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

    //listElements();
}

void ElementListModel::listElements()
{
    for(auto children : m_store->children()){
        printf("%s : %s : %s\n", children.get_value(m_records.m_element_name).c_str(), children.get_value(m_records.m_plugin).c_str(), children.get_value(m_records.m_package).c_str());
    }
    

}


void ElementListModel::addElement(GstPlugin* plugin, GstPluginFeature* element)
{
    auto iter = m_store->prepend();

    (*iter)[m_records.m_element_name] = gst_element_get_name(element); 
    (*iter)[m_records.m_plugin] = gst_plugin_get_name(plugin); 
    (*iter)[m_records.m_package] = gst_plugin_get_package(plugin);
}
