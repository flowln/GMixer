#include "gstreamer/Element.h"
#include "client/gtk/PipelineEditor.h"
#include "client/gtk/graph/ElementNode.h"
#include "client/gtk/graph/GraphViewer.h"
#include "client/gtk/graph/Pad.h"

#include <variant>

using namespace GMixer;

ElementNode* ElementNode::create(GraphViewer* parent, Element* element, int x, int y)
{
    auto node = new ElementNode(parent, element, x, y);
    node->updateNode();
    return node;
}

ElementNode* ElementNode::create(GraphViewer* parent, const gchar* element_name, int x, int y)
{
    return ElementNode::create(parent, new Element(element_name), x, y);
}

ElementNode::~ElementNode()
{
    if(m_properties)
        m_properties->destroy();
    delete m_element;
}

ElementNode::ElementNode(GraphViewer* parent, Element* element, int x, int y)
    : Node(parent, element->getName(), x, y)
    , m_element(element)
{
    auto sources = element->getSources();
    while(sources){
        addInputPad(new InputPad(this, GST_PAD( sources->data )));
        sources = sources->next; 
    }
    auto sinks = element->getSinks();
    while(sinks){
        addOutputPad(new OutputPad(this, GST_PAD( sinks->data )));
        sinks = sinks->next; 
    }
}

void ElementNode::updateNode()
{
    for(auto in : m_input_pads){
        if(in->isOutdated()){
            auto peer = in->getPeerOfBase();
            if(peer)
                in->link(peer);
        }
    }
    for(auto out : m_output_pads){
        if(out->isOutdated()){
            auto peer = out->getPeerOfBase();
            if(peer)
                out->link(peer);
        }
    }
}

bool ElementNode::operator==(Element* elem)
{
    return elem == m_element;
}

PropertyList* ElementNode::getProperties() 
{
    if(m_properties)
        return m_properties;

    auto ptr = new PropertyList();
    ptr->add(&m_name);

    guint num_props;
    auto props = m_element->getProperties(&num_props);
    for(guint i = 0; i < num_props; i++){
        auto prop = props[i];
        auto property = new GMixer::Property(g_param_spec_get_name(prop), true);

        property->addField("desc", g_param_spec_get_blurb(prop), G_TYPE_STRING);

        auto def = g_param_spec_get_default_value(prop);
        auto def_type = G_VALUE_TYPE( def );
        std::string str;

        if     (g_type_is_a(def_type, G_TYPE_STRING) )
            str = g_value_get_string(def) ? g_value_get_string(def) : "NULL";
        else if(g_type_is_a(def_type, G_TYPE_INT) )
            str = std::to_string(g_value_get_int(def));
        else if(g_type_is_a(def_type, G_TYPE_INT64) )
            str = std::to_string(g_value_get_int64(def));
        else if(g_type_is_a(def_type, G_TYPE_UINT) )
            str = std::to_string(g_value_get_uint(def));
        else if(g_type_is_a(def_type, G_TYPE_UINT64) )
            str = std::to_string(g_value_get_uint64(def));
        else if(g_type_is_a(def_type, G_TYPE_LONG) )
            str = std::to_string(g_value_get_long(def));
        else if(g_type_is_a(def_type, G_TYPE_ULONG) )
            str = std::to_string(g_value_get_ulong(def));
        else if(g_type_is_a(def_type, G_TYPE_FLOAT) )
            str = std::to_string(g_value_get_float(def));
        else if(g_type_is_a(def_type, G_TYPE_DOUBLE) )
            str = std::to_string(g_value_get_double(def));
        else if(g_type_is_a(def_type, G_TYPE_BOOLEAN) )
            str = std::to_string(g_value_get_boolean(def));
        // TODO: Add special enums / objects 

        if(!str.empty())
            property->addField("default", str, def_type); 

        property->connect(sigc::mem_fun(*this, &ElementNode::updateProperty));
        ptr->add(property);
    }

    g_free(props);

    m_properties = ptr;
    return ptr;
}

template <typename T>
void updateViaSscanf(const char* fmt, GstElement* base, const char* name, const char* value)
{
    T x;
    sscanf(value, fmt, &x);
    g_object_set(base, name, x, NULL);
}
template <>
void updateViaSscanf<double>(const char* fmt, GstElement* base, const char* name, const char* value)
{
    double x;
    sscanf(value, fmt, &x);
    printf("Str: %s | Double: %lf\n", value, x);
    g_object_set(base, name, x, NULL);
}

/** Compares two c strings, case-insensitive. 
 *  Only returns true/false for whether they are equal or not.*/
inline bool compareCaseInsensitive(const char* s1, const char* s2)
{
    int i = 0;
    while(s1[i] && s2[i]){
        if(tolower(s1[i]) != tolower(s2[i]))
            return false;
    }
    return true;
}

bool ElementNode::updateProperty(GMixer::Property* property, const std::string& field, const std::string& value)
{
    switch(property->getType(field)){
        case G_TYPE_STRING:
            g_object_set(m_element->getBase(), property->getName().c_str(), value.c_str(), NULL);
            break;
        case G_TYPE_INT:
            updateViaSscanf<int>("%d", m_element->getBase(), property->getName().c_str(), value.c_str());
            break;
        case G_TYPE_INT64:
            updateViaSscanf<long long>("%Ld", m_element->getBase(), property->getName().c_str(), value.c_str());
            break;
        case G_TYPE_UINT:
            updateViaSscanf<unsigned int>("%u", m_element->getBase(), property->getName().c_str(), value.c_str());
            break;
        case G_TYPE_UINT64:
            updateViaSscanf<unsigned long long>("%Lu", m_element->getBase(), property->getName().c_str(), value.c_str());
            break;
        case G_TYPE_LONG:
            updateViaSscanf<long>("%ld", m_element->getBase(), property->getName().c_str(), value.c_str());
            break;
        case G_TYPE_ULONG:
            updateViaSscanf<unsigned long>("%lu", m_element->getBase(), property->getName().c_str(), value.c_str());
            break;
        case G_TYPE_FLOAT:
            updateViaSscanf<float>("%f", m_element->getBase(), property->getName().c_str(), value.c_str());
            break;
        case G_TYPE_DOUBLE:
            updateViaSscanf<double>("%lf", m_element->getBase(), property->getName().c_str(), value.c_str());
            break;
        case G_TYPE_BOOLEAN:
            g_object_set(m_element->getBase(), property->getName().c_str(), 
                    value != "0" && !compareCaseInsensitive(value.c_str(), "false"), NULL);
            break;
        default:
            g_printerr("[ERROR] This type is not yet implemented. Value will not be set.");
            return false;
    }

    return true;
}

Pad* ElementNode::searchForPeer(GstPad* pad)
{
    for(auto in : m_input_pads){
        if(GST_PAD_PEER( pad ) == in->getBase()) return in;
    }
    for(auto out : m_output_pads){
        if(GST_PAD_PEER( pad ) == out->getBase()) return out;
    }
    return nullptr;
}
