#include "views/graph/Node.h"
#include "views/graph/Pad.h"
#include "views/graph/ElementNode.h"
#include "views/graph/GraphViewer.h"

Pad::Pad(Node* parent, GstPad* base)
    : m_parent(parent)
    , m_base(base)
{
}

bool Pad::operator==(GstPad* pad)
{
    return m_base == pad;
}

void Pad::draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    cr->save();

    cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);
    cr->rectangle(m_last_x - m_last_width / 2, m_last_y - m_last_height / 2, m_last_width, m_last_height);
    cr->fill();
    if(m_staging_link){
        cr->set_source_rgba(0.1, 0.1, 0.1, 0.8);
        std::vector<double> dashes = {10.0};
        cr->set_dash(dashes, 0);

        cr->move_to(m_last_x, m_last_y);
        cr->line_to(GraphViewer::cursor_pos_x, GraphViewer::cursor_pos_y);
        cr->stroke();
    }
    if(isLinked() && m_initiator){
        cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);

        cr->move_to(m_last_x, m_last_y);
        cr->line_to(m_peer->lastX(), m_peer->lastY());
        cr->stroke();
    }

    cr->restore();
}

Pad* Pad::getPeerOfBase()
{
    auto peer_parent = GST_ELEMENT( gst_pad_get_parent( GST_PAD_PEER(getBase())) );
    auto peer_node = m_parent->getGraph()->searchForElement(peer_parent);
    if(!peer_node)
        return nullptr;
    auto peer_pad = peer_node->searchForPeer(getBase());
    gst_object_unref(peer_parent);
    return peer_pad;
}

void InputPad::link(Pad* peer, bool initiator)
{
    auto out = dynamic_cast<OutputPad*>(peer);
    if(!out)
        return;

    if(isLinked())
        unlink();

    m_peer = out;
    m_initiator = initiator;

    if(initiator)
        out->link(this, false);
}
void InputPad::unlink(bool initiator)
{
    if(initiator)
        m_peer->unlink(false);

    m_peer = nullptr;
}

void OutputPad::link(Pad* peer, bool initiator)
{
    auto in = dynamic_cast<InputPad*>(peer);
    if(!in)
        return;

    m_peer = in;
    m_initiator = initiator;

    if(initiator)
        in->link(this, false);
}
void OutputPad::unlink(bool initiator)
{
    if(initiator)
        m_peer->unlink(false);

    m_peer = nullptr;
}
