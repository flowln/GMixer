#include "views/graph/Node.h"

#define IO_VERTICAL_SIZE   0.1 
#define IO_HORIZONTAL_SIZE 0.1 * m_height_by_width

Node::Node(Glib::ustring name, int x, int y)
    : m_name(name)
    , m_x(x), m_y(y)
{}

void Node::setPosition(int new_x, int new_y)
{
    m_x = new_x; 
    m_y = new_y; 
    update_callback(); 
}

void Node::onClick(double x, double y)
{
    auto n = numberOfIOPad(x, y);
    if(n < 0){
        select();
        return;
    }

    bool is_input = n < m_inputs;
    int index = is_input ? n : n - m_inputs;
    link_callback(is_input, this, index);
    update_callback();
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

    if(m_inputs > 0)
        drawInputs(cr);
    if(m_outputs > 0)
        drawOutputs(cr);

    cr->fill();

    cr->restore();

    drawName(cr);

    cr->restore();
}

void Node::select(){}

bool Node::contains(double x, double y) const
{
    return  (x >= m_x && x <= m_x + m_width) && 
            (y >= m_y && y <= m_y + m_height);
}
        
int Node::numberOfIOPad(double x, double y) const
{
    if(x <= m_x + IO_HORIZONTAL_SIZE * m_width){
        // In inputs
        double step = 1.0 / (m_inputs + 1);
        double off  = (y - m_y) / (step * m_height);
        if(std::abs(off - std::round(off)) <= IO_VERTICAL_SIZE / (2*step))
            return std::round(off) - 1;
    } 
    else if(x >= m_x + m_width - IO_HORIZONTAL_SIZE * m_width){
        // In outputs
        double step = 1.0 / (m_outputs + 1);
        double off  = (y - m_y) / (step * m_height);
        if(std::abs(off - std::round(off)) <= IO_VERTICAL_SIZE / (2*step))
            return m_inputs + std::round(off) - 1;
    }

    return -1;
}

std::pair<double, double> Node::IOPadToPosition(bool is_input, int index_pad) const
{
    double x_pos = -1.0;
    double y_pos = -1.0;
    if(is_input){
        x_pos = m_x + IO_HORIZONTAL_SIZE / 2;
        auto y_step = m_height / (m_inputs + 1);
        y_pos = m_y + y_step * (index_pad + 1);
    }
    else{
        x_pos = m_x + m_width - IO_HORIZONTAL_SIZE / 2;
        auto y_step = m_height / (m_outputs + 1);
        y_pos = m_y + y_step * (index_pad + 1);
    }

    return {x_pos, y_pos};
}

void Node::setSize(int new_width, int new_height)
{
    m_width = new_width;
    m_height = new_height;
    m_height_by_width = ((double) m_height) / m_width;
    update_callback();
}

inline void Node::drawInputs(const Cairo::RefPtr<Cairo::Context>& cr) const
{
    double step = 1.0 / (m_inputs + 1);
    auto temp_y = step;

    for(int i = 0; i < m_inputs; i++){
        cr->rectangle(0.0, temp_y - IO_VERTICAL_SIZE / 2, IO_HORIZONTAL_SIZE, IO_VERTICAL_SIZE);
        temp_y += step;
    }

}

inline void Node::drawOutputs(const Cairo::RefPtr<Cairo::Context>& cr) const
{
    double step = 1.0 / (m_outputs + 1);
    auto temp_y = step;

    for(int i = 0; i < m_outputs; i++){
        cr->rectangle(1.0 - IO_HORIZONTAL_SIZE, temp_y - IO_VERTICAL_SIZE / 2, IO_HORIZONTAL_SIZE, IO_VERTICAL_SIZE);
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
