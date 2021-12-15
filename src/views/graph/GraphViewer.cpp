#include "views/graph/GraphViewer.h"

#include <gtkmm/gestureclick.h>
#include <cairomm/cairomm.h>

GraphViewer::GraphViewer()
    : Gtk::DrawingArea()
{
    set_vexpand(true);
    set_margin(10);

    auto controller = Gtk::GestureClick::create();
    controller->signal_pressed().connect(sigc::mem_fun(*this, &GraphViewer::pressed), false);
    add_controller(controller);

    set_draw_func(sigc::mem_fun(*this, &GraphViewer::draw));
    m_nodes.push_back(new Node("one", 0, 0));
    m_nodes.push_back(new Node("two", 500, 0));
    m_nodes.push_back(new Node("three", 500, 200));
    for(auto node : m_nodes)
        node->onUpdateCallback([this]{queue_draw();});
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

    cr->fill();
}
void GraphViewer::pressed(int n, double x, double y)
{
    for(auto node : m_nodes){
        if(node->contains(x, y)){
            node->onClick(x, y);
            break;
        }
    }
}
