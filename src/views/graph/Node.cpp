#include "signals/Graph.h"
#include "views/graph/Node.h"
#include "views/graph/Pad.h"
#include "views/graph/GraphViewer.h"

#define node_to_pad_width   0.1
#define node_to_pad_height  0.2

using namespace GMixer;

void Node::deselect()
{ 
    m_is_selected = false; 
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
}

const std::unique_ptr<PropertyList> Node::getProperties() 
{
    auto ptr = std::make_unique<PropertyList>();
    ptr->add(&m_name);
    return ptr;
}

Node::Node(GraphViewer* parent, Glib::ustring name, int x, int y)
    : m_parent(parent)
    , m_name(name)
{
    setSize(160.0, 80.0);
    setPosition(x, y);
}

void Node::setPosition(int new_x, int new_y)
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
    select();

    if(x <= m_x + 0.1*m_width){
        InputPad* closest = m_input_pads.at(0);
        for(auto in : m_input_pads){
            closest = (closest->lastY() - y) < (in->lastY() - y) ? closest : in;
        }
        closest->stageLinking();
        m_selected_pad = closest;
    }
    else if(x >= m_x + 0.9*m_width){
        OutputPad* closest = m_output_pads.at(0);
        for(auto out : m_output_pads){
            closest = (closest->lastY() - y) < (out->lastY() - y) ? closest : out;
        }
        closest->stageLinking();
        m_selected_pad = closest;
    }
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
            in->draw(cr);
    }
    if(numOfOutputs() > 0){
        for(auto out : m_output_pads)
            out->draw(cr);
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
    pad->setPosition(m_x + m_width * node_to_pad_width / 2, m_y + (i+1)*step);
}
void Node::updateOutputPadGeometry(OutputPad* pad, int start_index)
{
    pad->setSize(m_width * node_to_pad_width, m_height * node_to_pad_height);

    double step = m_height / (numOfOutputs() + 1);
    int i = start_index >= 0 ? start_index : numOfOutputs() - 1;
    for(; i >= 0; i--) if(m_output_pads.at(i) == pad) break;
    pad->setPosition(m_x + m_width - m_width * node_to_pad_width / 2, m_y + (i+1)*step);
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
