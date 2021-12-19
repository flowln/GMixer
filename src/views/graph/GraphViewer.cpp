#include "backend/Element.h"
#include "views/PipelineEditor.h"
#include "views/graph/GraphViewer.h"
#include "views/graph/Node.h"
#include "views/graph/ElementNode.h"
#include "views/graph/Pad.h"

#include <cairomm/cairomm.h>

double GraphViewer::cursor_pos_x = 0, GraphViewer::cursor_pos_y = 0;

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

ElementNode* GraphViewer::searchForElement(GstElement* elem)
{
    for(auto node : m_nodes){
        auto elem_node = dynamic_cast<ElementNode*>(node);
        if(elem_node && *elem_node->getElement() == elem) {
            return elem_node;
    }}
    return nullptr;
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

    cr->fill();
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
                if(m_selected_node && m_selected_node != node){
                    if(m_selected_node->selectedPad()){
                        node->selectedPad()->link(m_selected_node->selectedPad());
                        node->deselect();
                    }

                    m_selected_node->deselect();
                }
                m_selected_node = node;
                just_selected = true;

                break;
            }
        }
        if(!just_selected && m_selected_node){
            m_selected_node->deselect();
            m_selected_node = nullptr;
        }
    }
    else if(button == 3){ // Right click

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
    m_selected_start_x = m_selected_node->getX();
    m_selected_start_y = m_selected_node->getY();
    m_is_dragging = true;
}  
void GraphViewer::updateDrag(double offset_x, double offset_y)
{
    if(!m_is_dragging)
        return;

    m_selected_node->setPosition(m_selected_start_x + offset_x, m_selected_start_y + offset_y);
}  
void GraphViewer::endDrag(double offset_x, double offset_y)
{
    if(!m_is_dragging)
        return;

    m_is_dragging = false;
}  

void GraphViewer::moved(double x, double y)
{
    cursor_pos_x = x;
    cursor_pos_y = y;
    queue_draw();
}
