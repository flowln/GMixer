#include "client/gtk/graph/Node.h"
#include "client/gtk/graph/Pad.h"
#include "client/gtk/graph/ElementNode.h"
#include "client/gtk/graph/GraphViewer.h"

Pad::Pad(Node* parent, GstPad* base)
    : m_parent(parent)
    , m_base(base)
{
}

bool Pad::operator==(GstPad* pad)
{
    return m_base == pad;
}

void Pad::draw(const Cairo::RefPtr<Cairo::Context>& cr, bool is_mouse_over)
{
    cr->save();
    cr->save();

    cr->translate(m_x, m_y);
    cr->scale(m_width, m_height);

    if(is_mouse_over){
        cr->set_source_rgba(0.2, 0.2, 0.2, 1.0);
    }
    else{
        cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);
    }
    cr->rectangle(0, 0, 1, 1);
    cr->fill();
    cr->restore();

    if(m_staging_link){
        cr->set_source_rgba(0.1, 0.1, 0.1, 0.8);
        std::vector<double> dashes = {10.0};
        cr->set_dash(dashes, 0);

        cr->move_to(m_x + m_width / 2, m_y + m_height / 2);
        cr->line_to(GraphViewer::cursor_pos_x, GraphViewer::cursor_pos_y);
        cr->stroke();
    }
    if(isLinked() && m_initiator){
        cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);

        cr->move_to(m_x + m_width / 2, m_y + m_height / 2);
        cr->line_to(m_peer->lastX() + m_peer->lastWidth() / 2, m_peer->lastY() + m_peer->lastHeight() / 2);
        cr->stroke();
    }

    cr->restore();

}

bool Pad::contains(double x, double y) const
{
    return  (x >= m_x && x <= m_x + m_width) && 
            (y >= m_y && y <= m_y + m_height);
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

    if(initiator){
        if(isLinked())
            unlink();
        out->link(this, false);
        if(m_base && peer && peer->getBase() && !isOutdated())
            gst_pad_link(peer->getBase(), m_base);
    }

    m_peer = out;
    m_initiator = initiator;
}
void InputPad::unlink(bool initiator)
{
    if(initiator){
        m_peer->unlink(false);
        if(m_base)
            gst_pad_unlink(m_peer->getBase(), m_base);
    }

    m_peer = nullptr;
}

void OutputPad::link(Pad* peer, bool initiator)
{
    auto in = dynamic_cast<InputPad*>(peer);
    if(!in)
        return;

    if(initiator){
        if(isLinked())
            unlink();
        in->link(this, false);
        if(m_base && peer && peer->getBase() && !isOutdated())
            gst_pad_link(m_base, peer->getBase());
    }

    m_peer = in;
    m_initiator = initiator;
}
void OutputPad::unlink(bool initiator)
{
    if(initiator){
        m_peer->unlink(false);
        if(m_base)
            gst_pad_unlink(m_base, m_peer->getBase());
    }

    m_peer = nullptr;
}
