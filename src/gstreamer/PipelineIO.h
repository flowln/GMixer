#pragma once

#include <gtkmm/window.h>

// Forward-declaration
class Pipeline;

class PipelineFactory{
    public:
        static void setMainWindow(Gtk::Window* main_window) { s_main_window = main_window; };
        static void createPipeline();
        static void createPipelineFromFile();
    private:
        static Gtk::Window* s_main_window;
};

namespace PipelineSaver{
    void saveCurrentPipeline();
    void savePipeline(Pipeline*);
}
