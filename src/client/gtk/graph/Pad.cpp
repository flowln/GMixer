#include "client/gtk/graph/Pad.h"
#include "client/gtk/graph/ElementNode.h"
#include "client/gtk/graph/GraphViewer.h"
#include "client/gtk/graph/Node.h"

Pad::Pad(Node* parent, GstPad* base, Availability aval) : m_availability(aval), m_parent(parent), m_base(base) {}

void Pad::destroy()
{
    if (isLinked())
        unlink();

    delete this;
}

bool Pad::operator==(GstPad* pad)
{
    return m_base == pad;
}

static const std::vector<double> dashes = { 0.15 };
void Pad::draw(const Cairo::RefPtr<Cairo::Context>& cr, bool is_mouse_over)
{
    cr->save();

    cr->translate(m_x, m_y);
    cr->scale(m_width, m_height);

    if (is_mouse_over) {
        cr->set_source_rgba(0.2, 0.2, 0.2, 1.0);
    } else {
        switch (m_availability) {
            case Availability::ALWAYS:
                cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);
                break;
            case Availability::SOMETIMES:
                cr->set_source_rgba(0.5, 0.1, 0.1, 1.0);
                cr->set_dash(dashes, 0);
                break;
            case Availability::ON_DEMAND:
                cr->set_source_rgba(0.1, 0.5, 0.1, 1.0);
                cr->set_dash(dashes, 0);
                break;
        }
    }
    cr->rectangle(0, 0, 1, 1);
    if (m_availability == Availability::ALWAYS)
        cr->fill();
    else {
        cr->set_line_width(0.15);
        cr->stroke();
    }

    cr->set_line_width(0.2);

    if (m_staging_link) {
        cr->set_source_rgba(0.1, 0.1, 0.1, 0.8);
        cr->set_dash(dashes, 0);

        cr->move_to(0.5, 0.5);
        cr->line_to((GraphViewer::cursor_pos_x - m_x) / m_width, (GraphViewer::cursor_pos_y - m_y) / m_height);
        cr->stroke();
    }
    if (isLinked() && m_initiator) {
        cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);

        cr->move_to(0.5, 0.5);
        cr->line_to((m_peer->lastX() - m_x + m_peer->lastWidth() / 2) / m_width,
                    (m_peer->lastY() - m_y + m_peer->lastHeight() / 2) / m_height);
        cr->stroke();
    }

    cr->restore();
}

bool Pad::contains(double x, double y) const
{
    return (x >= m_x && x <= m_x + m_width) && (y >= m_y && y <= m_y + m_height);
}

Pad* Pad::getPeerOfBase()
{
    auto peer_parent = GST_ELEMENT(gst_pad_get_parent(GST_PAD_PEER(getBase())));
    auto peer_node   = m_parent->getGraph()->searchForElement(peer_parent);
    if (!peer_node)
        return nullptr;
    auto peer_pad = peer_node->searchForPeer(getBase());
    gst_object_unref(peer_parent);
    return peer_pad;
}

void InputPad::link(Pad* peer, bool initiator)
{
    auto out = dynamic_cast<OutputPad*>(peer);
    if (!out)
        return;

    if (initiator) {
        if (isLinked())
            unlink();
        out->link(this, false);
        if (m_base && peer && peer->getBase() && !isOutdated())
            gst_pad_link(peer->getBase(), m_base);
    }

    m_peer         = out;
    m_initiator    = initiator;
    m_staging_link = false;
}
void InputPad::unlink(bool initiator)
{
    if (initiator) {
        m_peer->unlink(false);
        if (m_base)
            gst_pad_unlink(m_peer->getBase(), m_base);
    }

    m_peer = nullptr;
}

void OutputPad::link(Pad* peer, bool initiator)
{
    auto in = dynamic_cast<InputPad*>(peer);
    if (!in)
        return;

    if (initiator) {
        if (isLinked())
            unlink();
        in->link(this, false);
        if (m_base && peer && peer->getBase() && !isOutdated())
            gst_pad_link(m_base, peer->getBase());
    }

    m_peer         = in;
    m_initiator    = initiator;
    m_staging_link = false;
}
void OutputPad::unlink(bool initiator)
{
    if (initiator) {
        m_peer->unlink(false);
        if (m_base)
            gst_pad_unlink(m_base, m_peer->getBase());
    }

    m_peer = nullptr;
}
