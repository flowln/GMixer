#include "gstreamer/Element.h"
#include "views/PipelineEditor.h"
#include "views/graph/ElementNode.h"
#include "views/graph/GraphViewer.h"
#include "views/graph/Pad.h"

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
    auto ptr = new PropertyList();
    ptr->add(&m_name);

    guint num_props;
    auto props = m_element->getProperties(&num_props);
    for(guint i = 0; i < num_props; i++){
        auto prop = props[i];
        auto property = new Property(g_param_spec_get_name(prop), true);

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

    return ptr;
}
bool ElementNode::updateProperty(Property* property, const std::string& updated_field, const std::string& updated_value)
{
    printf("Property %s was updated: %s = %s\n", property->getName().c_str(), updated_field.c_str(), updated_value.c_str());
    g_object_set(m_element->getBase(), property->getName().c_str(), updated_value.c_str(), NULL);
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
