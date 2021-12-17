#include "ElementUtils.h"
#include <gst/gst.h>

const plugin_info* ElementUtils::getPluginInfo(const Glib::ustring& name)
{
    auto plugin = gst_plugin_load_by_name(name.c_str());
    if(!plugin)
        return nullptr;
    
    auto res = new plugin_info();

    res->description = gst_plugin_get_description(plugin);
    res->version = gst_plugin_get_version(plugin);
    res->license = gst_plugin_get_license(plugin);

    g_object_unref(plugin);

    return res;
}

const element_info* ElementUtils::getElementInfo(const Glib::ustring& name)
{
    auto factory = gst_element_factory_find(name.c_str());

    auto res = new element_info();

    res->description = gst_element_factory_get_description(factory);
    res->author = gst_element_factory_get_author(factory);

    auto src_pads = new std::list<pad_info>(2);
    auto sink_pads = new std::list<pad_info>(2);

    guint num_srcs = 0;
    guint num_sinks = 0;

    auto templates = gst_element_factory_get_static_pad_templates(factory);
    while(templates){
        auto pad_template = (GstStaticPadTemplate*) (templates->data);
        
        auto pad_info = new struct pad_info();
        pad_info->caps = pad_template->static_caps.string;
        switch(pad_template->presence){
            case(GST_PAD_ALWAYS):
                pad_info->availability = "Always";
                break;
            case(GST_PAD_SOMETIMES):
                pad_info->availability = "Sometimes";
                break;
            case(GST_PAD_REQUEST):
                pad_info->availability = "Request";
                break;
        }

        if(pad_template->direction == GST_PAD_SRC){
            src_pads->push_front(*pad_info);
            num_srcs += 1;
        }
        else if(pad_template->direction == GST_PAD_SINK){
            sink_pads->push_front(*pad_info);
            num_sinks += 1;
        }

        templates = templates->next;
    }

    res->num_srcs = num_srcs;
    res->num_sinks = num_sinks;

    res->src_pads = src_pads;
    res->sink_pads = sink_pads;

    g_object_unref(factory);

    return res;
}

const element_info* ElementUtils::getElementInfo(const GstElement& element)
{
    auto res = new element_info();
    res->name = gst_element_get_name(&element);
    //TODO
    return res;
}
