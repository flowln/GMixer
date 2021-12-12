#pragma once

#include "backend/Pipeline.h"

#include <gtkmm/window.h>


class PipelineFactory{
    public:
        static void setMainWindow(Gtk::Window* main_window) { s_main_window = main_window; };
        static void createPipeline(sigc::slot<void(Glib::RefPtr<Pipeline>)> const&);
        static void createPipelineFromFile(sigc::slot<void(Glib::RefPtr<Pipeline>)> const&);
    private:
        static Gtk::Window* s_main_window;
};
