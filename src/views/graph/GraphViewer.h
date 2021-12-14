#pragma once

#include <gtkmm/drawingarea.h>

class GraphViewer : public Gtk::DrawingArea {
    public:
        GraphViewer();

        void draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
    private:
};
