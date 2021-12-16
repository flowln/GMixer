#include "views/graph/GraphViewer.h"

#include <cairomm/cairomm.h>

GraphViewer::GraphViewer()
    : Gtk::DrawingArea()
{
    set_vexpand(true);
    set_margin(10);

    m_click_controller = Gtk::GestureClick::create();
    m_click_controller->signal_pressed().connect(sigc::mem_fun(*this, &GraphViewer::pressed), false);
    m_click_controller->set_button(0);
    add_controller(m_click_controller);

    m_motion_controller = Gtk::EventControllerMotion::create();
    m_motion_controller->signal_motion().connect(sigc::mem_fun(*this, &GraphViewer::moved), false);
    add_controller(m_motion_controller);

    set_draw_func(sigc::mem_fun(*this, &GraphViewer::draw));
    m_nodes.push_back(new Node("one", 0, 0));
    m_nodes.push_back(new Node("two", 500, 0));
    m_nodes.push_back(new Node("three", 500, 200));
    for(auto node : m_nodes){
        node->onUpdateCallback([this]{ queue_draw(); });
        node->onLinkCallback([this](bool a, Node* b, int c){ link(a, b, c); });
    }
}

void GraphViewer::draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height)
{
    auto style_context = get_style_context();

    style_context->render_background(cr, 0, 0, width, height);

    auto color = style_context->get_color();
    cr->set_source_rgba(color.get_red(), color.get_green(), color.get_blue(), color.get_alpha());

    //Render graph here 
    for(auto node : m_nodes){
        node->draw(cr);
    }
    for(auto link : m_links){
        link->draw(cr);
    }
    if(m_constructing_link)
        m_constructing_link->draw(cr);

    cr->fill();
}

void GraphViewer::link(bool is_input, Node* node, int index)
{
    if(is_input){
        if(m_constructing_link && !m_constructing_link->input_node){
            m_constructing_link->input_node = node;
            m_constructing_link->index_input_pad = index;
            m_links.push_back(m_constructing_link);

            // TODO: Tell Gst to make the connection

            m_constructing_link = nullptr;
            return;
        }

        if(m_constructing_link){
            // Discart old incomplete link
            delete m_constructing_link;
        }
        m_constructing_link = new Link(node, index, nullptr, -1);
    }
    else{
        if(m_constructing_link && !m_constructing_link->output_node){
            m_constructing_link->output_node = node;
            m_constructing_link->index_output_pad = index;
            m_links.push_back(m_constructing_link);

            // TODO: Tell Gst to make the connection
            
            m_constructing_link = nullptr;
            return;
        }

        if(m_constructing_link){
            // Discart old incomplete link
            delete m_constructing_link;
        }
        m_constructing_link = new Link(nullptr, -1, node, index);
    }
}

void GraphViewer::pressed(int n, double x, double y)
{
    auto button = m_click_controller->get_current_button();

    if(button == 1){ //Left click
        for(auto node : m_nodes){
            if(node->contains(x, y)){
                node->onClick(x, y);
                break;
            }
        }
    }
    else if(button == 3){ // Right click
        if(m_constructing_link)
            delete m_constructing_link;
        m_constructing_link = nullptr;
    }
}

void GraphViewer::moved(double x, double y)
{
    Link::cursor_pos_x = x;
    Link::cursor_pos_y = y;
    queue_draw();
}
