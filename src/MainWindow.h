#pragma once

#include <gtkmm/grid.h>
#include <gtkmm/window.h>

#include "views/ElementSelector.h"
#include "views/PipelineSelector.h"

class MainWindow : public Gtk::Window {
    public:
        MainWindow();
        virtual ~MainWindow();
    private: 
        Gtk::Grid m_main_container;
        PipelineSelector* m_ps;
}; 
