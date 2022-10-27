#pragma once

#include "client/common/PipelineFactory.h"

#include <gtkmm/window.h>

// Forward-declaration
class Pipeline;

class Dialog{
public:
    // General dialog setup, must be done before creating any dialog
    static Gtk::Window* s_main_window;
    static PipelineFactory* s_factory;

    static void setMainWindow(Gtk::Window* main_window) { s_main_window = main_window; };
    static void setFactory(PipelineFactory* factory) 
    {
        if(s_factory)
            delete s_factory;
        s_factory = factory; 
    };

    // Pipeline specific dialogs
    static void createPipeline();
    static void importPipelineFromFile();

    static void savePipeline(Pipeline*);
};

class PipelineCreator{
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
