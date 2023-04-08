#pragma once

#include <gtkmm/window.h>

//Forward-declarations
class ElementSelector;
class PipelineEditor;
class PipelineSelector;
class Pipeline;

namespace Gtk {
class Grid;
class Paned;
}

class MainWindow final : public Gtk::Window {
    public:
        MainWindow();

        void changePipelineEditor(Pipeline* selected);
        void attachEditor(PipelineEditor& editor);
    private: 
        std::shared_ptr<Gtk::Grid> m_main_container;
        std::shared_ptr<Gtk::Paned> m_right_container;

        std::vector<std::shared_ptr<PipelineEditor>> m_editors;
}; 
