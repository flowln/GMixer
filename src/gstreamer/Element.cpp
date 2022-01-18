#include "gstreamer/Element.h"

Element::Element(const gchar* name)
    : Element(gst_element_factory_create(gst_element_factory_find(name), name))
{
}

Element::Element(GstElement* element)
    : m_element(element)
{
}

Element::~Element()
{
    g_object_unref(m_element);
}

const gchar* Element::getName() const
{
    return gst_element_get_name(m_element);
}

GList* Element::getSinks() const
{
    return GST_ELEMENT_CAST( m_element )->srcpads;
}
GList* Element::getSources() const
{
    return GST_ELEMENT_CAST( m_element )->sinkpads;
}
GParamSpec** Element::getProperties(guint* num_props) const
{
    auto props = g_object_class_list_properties(G_OBJECT_GET_CLASS( m_element ), num_props);
    return props;
}

int Element::isPadLinked(GstPad* pad)
{
    return gst_pad_is_linked(pad);
}
