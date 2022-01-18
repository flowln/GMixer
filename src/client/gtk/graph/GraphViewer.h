#pragma once

#include <gst/gstelement.h>

#include <gtkmm/drawingarea.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/gesturedrag.h>
#include <gtkmm/eventcontrollermotion.h>

//Forward-declaration
class ElementNode;
class PipelineGraph;
class Node;
enum class OperationMode;

class GraphViewer : public Gtk::DrawingArea {
    public:
        GraphViewer(PipelineGraph* parent);

        ElementNode* searchForElement(GstElement*);
        void addNode(Node* node);
        void removeNode(Node* node);

        OperationMode getMode();

        void draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        void pressed(int n, double x, double y);
        void moved(double x, double y);
        void beginDrag(double x, double y);
        void updateDrag(double offset_x, double offset_y);
        void endDrag(double offset_x, double offset_y);

        void beginMoveOperation(double x, double y);
        void beginCutOperation(double x, double y);
        void endCutOperation(double x, double y);

        static double cursor_pos_x, cursor_pos_y;
    private:
        PipelineGraph* m_parent;

        Glib::RefPtr<Gtk::GestureClick> m_click_controller;
        Glib::RefPtr<Gtk::GestureDrag> m_drag_controller;
        Glib::RefPtr<Gtk::EventControllerMotion> m_motion_controller;

        std::vector<Node*> m_nodes;
        
        Node* m_selected_node = nullptr;
        int m_move_start_x, m_move_start_y;
        int m_cut_start_x, m_cut_start_y;

        bool m_is_dragging = false;
        bool m_is_cutting = false;
};
