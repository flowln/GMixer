#include "gstreamer/Element.h"
#include "signals/Graph.h"
#include "client/gtk/PipelineEditor.h"
#include "client/gtk/graph/GraphViewer.h"
#include "client/gtk/graph/Node.h"
#include "client/gtk/graph/ElementNode.h"
#include "client/gtk/graph/Pad.h"

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
    if(m_is_cutting){
        cr->set_source_rgba(0.1, 0.1, 0.1, 0.8);
        std::vector<double> dashes = {10.0};
        cr->set_dash(dashes, 0);

        cr->move_to(m_cut_start_x, m_cut_start_y);
        cr->line_to(GraphViewer::cursor_pos_x, GraphViewer::cursor_pos_y);
        cr->stroke();
    }
}

void GraphViewer::pressed(int n, double x, double y)
{
    if(getMode() != OperationMode::MODE_SELECT)
        return;

    auto button = m_click_controller->get_current_button();

    if(button == 1){ //Left click
        for(auto node : m_nodes){
            if(node->contains(x, y)){
                node->onClick(x, y);
               
                if(node->isSelected()){
                    if(m_selected_node)
                        m_selected_node->deselect();
                    m_selected_node = node;
                    Signals::node_selected().emit(node);
                }
                else{ // A pad was selected
                    Pad* selected = node->selectedPad();
                    if(selected != m_linking_pad && m_linking_pad){
                        m_linking_pad->link(selected);
                        m_linking_pad = nullptr;
                    }
                    else
                        m_linking_pad = selected;
                }

                break;
            }
        }
    }
    else if(button == 3){ // Right click
        if(m_selected_node){
            m_selected_node->deselect();
            m_selected_node = nullptr;
            Signals::node_selected().emit(nullptr);
        }
    }

    queue_draw();
}
void GraphViewer::beginDrag(double x, double y)
{
    if(getMode() == OperationMode::MODE_MOVE){
        beginMoveOperation(x, y);
        Signals::node_selected().emit(m_selected_node);
    }
    else if(getMode() == OperationMode::MODE_CUT){
        beginCutOperation(x, y);
    } 
}  
void GraphViewer::updateDrag(double offset_x, double offset_y)
{
    if(m_is_dragging)
        m_selected_node->setPosition(m_move_start_x + offset_x, m_move_start_y + offset_y);

}  
void GraphViewer::endDrag(double offset_x, double offset_y)
{
    m_is_dragging = false;
    if(m_is_cutting)
        endCutOperation(offset_x, offset_y);
}  

void GraphViewer::moved(double x, double y)
{
    cursor_pos_x = x;
    cursor_pos_y = y;
    queue_draw();
}

void GraphViewer::beginMoveOperation(double x, double y)
{
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

    m_selected_node->onClick(x, y);
    m_move_start_x = m_selected_node->getX();
    m_move_start_y = m_selected_node->getY();
    m_is_dragging = true;
}

void GraphViewer::beginCutOperation(double x, double y)
{
    m_is_cutting = true;
    m_cut_start_x = x;
    m_cut_start_y = y;
}
void GraphViewer::endCutOperation(double offset_x, double offset_y)
{
    m_is_cutting = false;
    
    auto delta_cut = offset_y / offset_x;
    auto start_cut_y = m_cut_start_y - delta_cut*m_cut_start_x; 

    auto min_cut_x   = offset_x > 0 ? m_cut_start_x : m_cut_start_x + offset_x;
    auto max_cut_x   = min_cut_x + std::abs(offset_x);

    for(auto node : m_nodes){
        // Since there's no output-output connections, 
        // we don't have to iterate the output pads.
        for(auto in : node->getInputPads()){
            if(in->isLinked()){
                auto delta_link = (in->lastY() - in->getPeer()->lastY())/(in->lastX() - in->getPeer()->lastX());
                if(delta_link == delta_cut) continue;

                auto start_link_y = in->lastY() - delta_link*in->lastX();
                auto min_link_x = in->lastX() <= in->getPeer()->lastX() ? in->lastX() : in->getPeer()->lastX();
                auto max_link_x = in->lastX() + in->getPeer()->lastX() - min_link_x;

                auto dx = (start_cut_y - start_link_y) / (delta_link - delta_cut);
                //printf("(%lf,%d):(%lf,%lf)  ===  dx = %lf  :  y = %lf\n", min_cut_x, m_cut_start_y, max_cut_x, m_cut_start_y + offset_y, dx, start_cut_y + dx*delta_cut);
                if(dx >= min_cut_x && dx <= max_cut_x && dx >= min_link_x && dx <= max_link_x)
                    in->unlink();
            }
        }
    }
    queue_draw();
}
