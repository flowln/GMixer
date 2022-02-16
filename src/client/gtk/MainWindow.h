#pragma once

#include <gtkmm/grid.h>
#include <gtkmm/window.h>

//Forward-declarations
class ElementSelector;
class PipelineEditor;
class PipelineSelector;

class MainWindow : public Gtk::Window {
    public:
        MainWindow();
        ~MainWindow();

        void attachEditor(PipelineEditor& editor);
        std::vector<PipelineEditor*>& getEditors() { return m_editors; }
    private: 
        Gtk::Grid m_main_container;
        std::vector<PipelineEditor*> m_editors;
}; 
