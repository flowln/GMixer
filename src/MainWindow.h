#pragma once

#include <gtkmm/grid.h>
#include <gtkmm/window.h>

#include "views/ElementSelector.h"
#include "views/PipelineEditor.h"
#include "views/PipelineSelector.h"

class MainWindow : public Gtk::Window {
    public:
        MainWindow();
        virtual ~MainWindow();
    private: 
        Gtk::Grid m_main_container;

        ElementSelector* m_element_selector;
        PipelineEditor* m_pipeline_editor;
        PipelineSelector* m_pipeline_selector;
}; 
