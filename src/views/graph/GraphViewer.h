#pragma once

#include "views/graph/Node.h"
#include "views/graph/Link.h"

#include <gtkmm/drawingarea.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/gesturedrag.h>
#include <gtkmm/eventcontrollermotion.h>

//Forward-declaration
class PipelineGraph;
enum class OperationMode;

class GraphViewer : public Gtk::DrawingArea {
    public:
        GraphViewer(PipelineGraph* parent);

        Node* searchNodeWithName(Glib::ustring name);
        void addNode(Node* node);
        void removeNode(Node* node);

        OperationMode getMode();

        void draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
        void link(bool is_input, Node* node, int index);

        void pressed(int n, double x, double y);
        void beginDrag(double x, double y);
        void updateDrag(double offset_x, double offset_y);
        void endDrag(double offset_x, double offset_y);
        void moved(double x, double y);

    private:
        PipelineGraph* m_parent;

        Glib::RefPtr<Gtk::GestureClick> m_click_controller;
        Glib::RefPtr<Gtk::GestureDrag> m_drag_controller;
        Glib::RefPtr<Gtk::EventControllerMotion> m_motion_controller;

        std::vector<Node*> m_nodes;
        std::vector<Link*>  m_links; 
        
        Link* m_constructing_link = nullptr;
        Node* m_selected_node = nullptr;

        bool m_is_dragging = false;
};
