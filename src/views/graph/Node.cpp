#include "views/graph/Node.h"

Node::Node(Glib::ustring name, int x, int y)
    : m_name(name)
    , m_x(x), m_y(y)
{

}
void Node::setPosition(int new_x, int new_y)
{
    m_x = new_x; 
    m_y = new_y; 
    update_callback(); 
}

void Node::onUpdateCallback(sigc::slot<void ()> callback)
{
    update_callback = callback;
}

void Node::draw(const Cairo::RefPtr<Cairo::Context> &cr) const
{
    cr->save();
    cr->save();

    cr->translate(m_x, m_y);
    cr->scale(m_width, m_height);

    cr->rectangle(0, 0, 1, 1); 
    cr->fill();

    cr->set_source_rgb(0.1, 0.1, 0.1);

    if(m_sink_pads > 0)
        drawSinkPads(cr);
    if(m_source_pads > 0)
        drawSourcePads(cr);

    cr->fill();

    cr->restore();

    drawName(cr);

    cr->restore();
}

void Node::setSize(int new_width, int new_height)
{
    m_width = new_width;
    m_height = new_height;
    m_width_by_height = ((double) m_width) / m_height;
    update_callback();
}

inline void Node::drawSinkPads(const Cairo::RefPtr<Cairo::Context>& cr) const
{
    double step = 1.0 / (m_sink_pads + 1);
    auto temp_y = step;

    for(int i = 0; i < m_sink_pads; i++){
        cr->rectangle(0.0, temp_y - 0.05, 0.1*m_width_by_height, 0.1);
        temp_y += step;
    }

}

inline void Node::drawSourcePads(const Cairo::RefPtr<Cairo::Context>& cr) const
{
    double step = 1.0 / (m_source_pads + 1);
    auto temp_x = 1.0 - 0.1 * m_width_by_height;
    auto temp_y = step;

    for(int i = 0; i < m_source_pads; i++){
        cr->rectangle(temp_x, temp_y - 0.05, 1.0 - temp_x, 0.1);
        temp_y += step;
    }

}

inline void Node::drawName(const Cairo::RefPtr<Cairo::Context>& cr) const
{
    cr->set_source_rgb(0, 0, 0);
    cr->set_font_size(16);

    // Based on https://www.cairographics.org/samples/text_align_center/ by Øyvind Kolås
    Cairo::TextExtents t;
    cr->get_text_extents(m_name, t);

    auto middle_x = m_x + m_width / 2;
    auto middle_y = m_y + m_height / 2;

    auto corrected_x = middle_x - (t.width/2 + t.x_bearing);
    auto corrected_y = middle_y - (t.height/2 + t.y_bearing);

    cr->move_to(corrected_x, corrected_y);
    cr->show_text(m_name);

}
