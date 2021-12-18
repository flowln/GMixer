#include "backend/Element.h"
#include "views/PipelineEditor.h"
#include "views/graph/ElementNode.h"
#include "views/graph/GraphViewer.h"

ElementNode::ElementNode(GraphViewer* parent, Element* element, int x, int y)
    : Node(parent, element->getName(), x, y)
    , m_element(element)
{
    setNumInputs(element->getNumSinks());
    setNumOutputs(element->getNumSources());

    for(auto pad : element->getSinks()){
        if(Element::isPadLinked(pad)){
            auto peer_pad = GST_PAD_PEER( pad );
            auto name = gst_element_get_name(GST_PAD_PARENT( peer_pad ));
            auto peer = dynamic_cast<ElementNode*>(parent->searchNodeWithName(Glib::ustring::sprintf("%s", name)));
            if(!peer) break;
            parent->link(true, this, element->getIndexOfSink(pad));
            parent->link(false, peer, peer->getElement()->getIndexOfSource( peer_pad ));
        }
    }
    for(auto pad : element->getSources()){
        if(Element::isPadLinked(pad)){
            auto peer_pad = GST_PAD_PEER( pad );
            auto name = gst_element_get_name(GST_PAD_PARENT( peer_pad ));
            auto peer = dynamic_cast<ElementNode*>(parent->searchNodeWithName(Glib::ustring::sprintf("%s", name)));
            if(!peer) break;
            parent->link(false, this, element->getIndexOfSource(pad));
            parent->link(true, peer, peer->getElement()->getIndexOfSink( peer_pad ));
        }
    }

}

ElementNode::ElementNode(GraphViewer* parent, const gchar* element_name, int x, int y)
    : ElementNode(parent, new Element(element_name), x, y)
{}

void ElementNode::select()
{
    Node::select();

    switch(m_parent->getMode()){
    case(OperationMode::MODE_SELECT):
        //TODO: Open property tab on the side
        break;
    case(OperationMode::MODE_MOVE):
        break;
    }
}

void ElementNode::deselect()
{
    Node::deselect();

    switch(m_parent->getMode()){
    case(OperationMode::MODE_SELECT):
        //TODO: Close property tab on the side
        break;
    case(OperationMode::MODE_MOVE):
        break;
    }
}
