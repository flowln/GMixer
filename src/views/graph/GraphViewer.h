#pragma once

#include "views/graph/Node.h"
#include "views/graph/Link.h"

#include <gtkmm/drawingarea.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/eventcontrollermotion.h>

class GraphViewer : public Gtk::DrawingArea {
    public:
        GraphViewer();

        void draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
        void link(bool is_input, Node* node, int index);

        void pressed(int n, double x, double y);
        void moved(double x, double y);

    private:
        Glib::RefPtr<Gtk::GestureClick> m_click_controller;
        Glib::RefPtr<Gtk::EventControllerMotion> m_motion_controller;

        std::vector<Node*> m_nodes;
        std::vector<Link*>  m_links; 
        Link* m_constructing_link = nullptr;
};
