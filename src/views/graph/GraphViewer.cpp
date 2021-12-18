#include "views/PipelineEditor.h"
#include "views/graph/GraphViewer.h"

#include <cairomm/cairomm.h>

GraphViewer::GraphViewer(PipelineGraph* parent)
    : Gtk::DrawingArea()
    , m_parent(parent)
{
    set_vexpand(true);
    set_margin(10);

    m_click_controller = Gtk::GestureClick::create();
    m_click_controller->signal_pressed().connect(sigc::mem_fun(*this, &GraphViewer::pressed), false);
    m_click_controller->set_button(0);
    add_controller(m_click_controller);

    m_drag_controller = Gtk::GestureDrag::create();
    m_drag_controller->signal_drag_begin().connect(sigc::mem_fun(*this, &GraphViewer::beginDrag), false);
    m_drag_controller->signal_drag_update().connect(sigc::mem_fun(*this, &GraphViewer::updateDrag), false);
    m_drag_controller->signal_drag_end().connect(sigc::mem_fun(*this, &GraphViewer::endDrag), false);
    add_controller(m_drag_controller);

    m_motion_controller = Gtk::EventControllerMotion::create();
    m_motion_controller->signal_motion().connect(sigc::mem_fun(*this, &GraphViewer::moved), false);
    add_controller(m_motion_controller);

    set_draw_func(sigc::mem_fun(*this, &GraphViewer::draw));
}

Node* GraphViewer::searchNodeWithName(Glib::ustring name)
{
    for(auto node : m_nodes){
        if(node->getName() == name)
            return node;
    }

    return nullptr;
}

void GraphViewer::addNode(Node* node)
{
    m_nodes.push_back(node);
    node->onUpdateCallback([this]{ queue_draw(); });
    node->onLinkCallback([this](bool a, Node* b, int c){ link(a, b, c); });

    queue_draw();
}

void GraphViewer::removeNode(Node* node)
{

}

OperationMode GraphViewer::getMode()
{
    return m_parent->getMode();
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
    if(getMode() != OperationMode::MODE_SELECT)
        return;

    auto button = m_click_controller->get_current_button();

    if(button == 1){ //Left click
        bool just_selected = false;
        for(auto node : m_nodes){
            if(node->contains(x, y)){
                node->onClick(x, y);
                if(node->isSelected()){
                    if(m_selected_node && m_selected_node != node)
                        m_selected_node->deselect();
                    m_selected_node = node;
                    just_selected = true;
                }
                break;
            }
        }
        if(!just_selected && m_selected_node){
            m_selected_node->deselect();
            m_selected_node = nullptr;
        }
    }
    else if(button == 3){ // Right click
        if(m_constructing_link)
            delete m_constructing_link;
        m_constructing_link = nullptr;

        if(m_selected_node){
            m_selected_node->deselect();
            m_selected_node = nullptr;
        }
    }

    queue_draw();
}
void GraphViewer::beginDrag(double x, double y)
{
    if(getMode() != OperationMode::MODE_MOVE)
        return;

    for(auto node : m_nodes){
        if(node->contains(x, y)){
            if(m_selected_node)
                m_selected_node->deselect();
            m_selected_node = node;
            break;
        }
    }

    if(!m_selected_node)
        return; //TODO: Move viewport

    m_selected_node->select();
    m_is_dragging = true;
}  
void GraphViewer::updateDrag(double offset_x, double offset_y)
{
    if(!m_is_dragging)
        return;

    m_selected_node->changePosition(offset_x, offset_y);
}  
void GraphViewer::endDrag(double offset_x, double offset_y)
{
    if(!m_is_dragging)
        return;

    m_selected_node->setPosition();
    m_is_dragging = false;
}  

void GraphViewer::moved(double x, double y)
{
    Link::cursor_pos_x = x;
    Link::cursor_pos_y = y;
    if(m_constructing_link)
        queue_draw();
}
