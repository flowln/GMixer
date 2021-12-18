#include "backend/Element.h"

Element::Element(const gchar* name)
    : Element(gst_element_factory_create(gst_element_factory_find(name), name))
{}

Element::Element(GstElement* element)
    : m_element(element)
{
    gst_element_foreach_pad(element, &Element::countPads, this);
}

Element::~Element()
{
    g_object_unref(m_element);
}

const gchar* Element::getName() const
{
    return gst_element_get_name(m_element);
}
int Element::getIndexOfSink(GstPad* sink) const
{
    int i = 0;
    for(auto pad : m_sinks){
        if(pad == sink) 
            return i;
        i += 1;
    }
    return -1;
}

int Element::getIndexOfSource(GstPad* source) const
{
    int i = 0;
    for(auto pad : m_sources){
        if(pad == source) 
            return i;
        i += 1;
    }
    return -1;

}

int Element::isPadLinked(GstPad* pad)
{
    return gst_pad_is_linked(pad);
}

gboolean Element::countPads(GstElement*, GstPad* pad, gpointer instance)
{
    if(GST_PAD_IS_SINK( pad )) ((Element*) instance)->addSink(pad);
    else ((Element*) instance)->addSource(pad);

    return TRUE;
}
