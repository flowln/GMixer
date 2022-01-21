#include "signals/Graph.h"
#include "client/gtk/graph/Node.h"
#include "client/gtk/graph/Pad.h"
#include "client/gtk/graph/GraphViewer.h"

#define node_to_pad_width   0.1
#define node_to_pad_height  0.2

using namespace GMixer;

void Node::deselect()
{ 
    if(!m_is_selected)
        return;
    m_is_selected = false; 
    stopLinking();

    update_callback();
}
void Node::stopLinking()
{
    if(!m_selected_pad)
        return;

    m_selected_pad = nullptr;

    for(int i = numOfInputs() - 1; i >= 0; i--)
        m_input_pads.at(i)->cancelLinking();
    for(int i = numOfOutputs() - 1; i >= 0; i--)
        m_output_pads.at(i)->cancelLinking();
}
void Node::select()
{ 
    m_is_selected = true; 
    Signals::node_selected().emit(this);

    update_callback();
}

PropertyList* Node::getProperties() 
{
    auto ptr = new PropertyList();
    ptr->add(&m_name);
    return ptr;
}

Node::Node(GraphViewer* parent, Glib::ustring name, double x, double y)
    : m_parent(parent)
    , m_name(name)
{
    setSize(160.0, 80.0);
    setPosition(x, y);
}

void Node::setPosition(double new_x, double new_y)
{
    m_x = new_x;
    m_y = new_y;

    for(int i = numOfInputs() - 1; i >= 0; i--)
        updateInputPadGeometry(m_input_pads.at(i), i); 
    for(int i = numOfOutputs() - 1; i >= 0; i--)
        updateOutputPadGeometry(m_output_pads.at(i), i); 

    update_callback(); 
}

void Node::addInputPad(InputPad* pad)
{ 
    m_input_pads.push_back(pad); 
    for(int i = numOfInputs() - 1; i >= 0; i--)
        updateInputPadGeometry(m_input_pads.at(i), i); 
}
void Node::addOutputPad(OutputPad* pad) 
{ 
    m_output_pads.push_back(pad); 
    for(int i = numOfOutputs() - 1; i >= 0; i--)
        updateOutputPadGeometry(m_output_pads.at(i), i); 
}

void Node::onClick(double x, double y)
{
    Pad* selected = nullptr;

    // Check if one input pad was clicked
    if(x <= m_x + node_to_pad_width*m_width){
        for(auto in : m_input_pads){
            if(!in->contains(x, y)) continue;

            selected = in;
            break;
        }
    }

    // Check if one output pad was clicked
    else if(x >= m_x + (1-node_to_pad_width)*m_width){
        for(auto out : m_output_pads){
            if(!out->contains(x, y)) continue;
            
            selected = out;
            break;
        }
    }

    // If no pad was clicked
    if(!selected){
        select();
        return;
    }

    selected->stageLinking();
    m_selected_pad = selected;
}

void Node::draw(const Cairo::RefPtr<Cairo::Context> &cr) const
{
    cr->save(); // Pre-draw
    cr->save(); // Pre-scale

    cr->translate(m_x, m_y);
    cr->scale(m_width, m_height);

    cr->rectangle(0, 0, 1, 1); 
    cr->fill();

    cr->restore(); // Pre-scale
    
    if(numOfInputs() > 0){
        for(auto in : m_input_pads)
            in->draw(cr, in->contains(m_parent->cursor_pos_x, m_parent->cursor_pos_y));
    }
    if(numOfOutputs() > 0){
        for(auto out : m_output_pads)
            out->draw(cr, out->contains(m_parent->cursor_pos_x, m_parent->cursor_pos_y));
    }

    cr->save();    // Pre-border
    // FIXME: Do this while scaled
    if(m_is_selected){
        cr->set_source_rgb(0.1, 0.1, 0.5);
        cr->set_line_width(4);
        cr->rectangle(m_x, m_y, m_width, m_height);
        cr->stroke();
    }
    else{
        cr->set_source_rgb(0, 0, 0);
        cr->rectangle(m_x, m_y, m_width, m_height);
        cr->stroke();
    }
    cr->restore(); // Pre-border


    drawName(cr);

    cr->restore(); // Pre-draw
}

bool Node::contains(double x, double y) const
{
    return  (x >= m_x && x <= m_x + m_width) && 
            (y >= m_y && y <= m_y + m_height);
}
        
void Node::setSize(double new_width, double new_height)
{
    m_width = new_width;
    m_height = new_height;

    for(auto in : m_input_pads)
        in->setSize(new_width * node_to_pad_width, new_height * node_to_pad_height);
    for(auto out : m_output_pads)
        out->setSize(new_width * node_to_pad_width, new_height * node_to_pad_height);

    update_callback();
}

void Node::updateInputPadGeometry(InputPad* pad, int start_index)
{
    pad->setSize(m_width * node_to_pad_width, m_height * node_to_pad_height);

    double step = m_height / (numOfInputs() + 1);
    int i = start_index >= 0 ? start_index : numOfInputs() - 1;
    for(; i >= 0; i--) if(m_input_pads.at(i) == pad) break;
    pad->setPosition(m_x, m_y + (i+1)*step - m_height * node_to_pad_height / 2);
}
void Node::updateOutputPadGeometry(OutputPad* pad, int start_index)
{
    pad->setSize(m_width * node_to_pad_width, m_height * node_to_pad_height);

    double step = m_height / (numOfOutputs() + 1);
    int i = start_index >= 0 ? start_index : numOfOutputs() - 1;
    for(; i >= 0; i--) if(m_output_pads.at(i) == pad) break;
    pad->setPosition(m_x + m_width - m_width * node_to_pad_width, m_y + (i+1)*step - m_height * node_to_pad_height / 2);
}

inline void Node::drawName(const Cairo::RefPtr<Cairo::Context>& cr) const
{
    cr->set_source_rgb(0, 0, 0);
    cr->set_font_size(16);

    // Based on https://www.cairographics.org/samples/text_align_center/ by Øyvind Kolås
    Cairo::TextExtents t;
    cr->get_text_extents(getName().c_str(), t);

    auto middle_x = m_x + m_width / 2;
    auto middle_y = m_y + m_height / 2;

    auto corrected_x = middle_x - (t.width/2 + t.x_bearing);
    auto corrected_y = middle_y - (t.height/2 + t.y_bearing);

    cr->move_to(corrected_x, corrected_y);
    cr->show_text(getName().c_str());

}
