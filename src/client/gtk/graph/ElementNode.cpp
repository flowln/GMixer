#include "client/gtk/graph/ElementNode.h"
#include "client/gtk/PipelineEditor.h"
#include "client/gtk/graph/GraphViewer.h"
#include "client/gtk/graph/Pad.h"

#include <variant>

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
    delete m_element;
}

ElementNode::ElementNode(GraphViewer* parent, Element* element, int x, int y)
    : Node(parent, element->getName(), x, y), m_element(element)
{
    auto sources = element->getSources();
    while (sources) {
        addInputPad(new InputPad(this, GST_PAD(sources->data)));
        sources = sources->next;
    }
    auto sinks = element->getSinks();
    while (sinks) {
        addOutputPad(new OutputPad(this, GST_PAD(sinks->data)));
        sinks = sinks->next;
    }
}

void ElementNode::updateNode()
{
    for (auto in : m_input_pads) {
        if (in->isOutdated()) {
            auto peer = in->getPeerOfBase();
            if (peer)
                in->link(peer);
        }
    }
    for (auto out : m_output_pads) {
        if (out->isOutdated()) {
            auto peer = out->getPeerOfBase();
            if (peer)
                out->link(peer);
        }
    }
}

bool ElementNode::operator==(Element* elem)
{
    return elem == m_element;
}

std::vector<Element::Property*>& ElementNode::getProperties()
{
    return m_element->getProperties();
}

Pad* ElementNode::searchForPeer(GstPad* pad)
{
    for (auto in : m_input_pads) {
        if (GST_PAD_PEER(pad) == in->getBase())
            return in;
    }
    for (auto out : m_output_pads) {
        if (GST_PAD_PEER(pad) == out->getBase())
            return out;
    }
    return nullptr;
}
