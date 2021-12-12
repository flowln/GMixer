/*  This view shows the available pipelines at the moment
 *  in the form of a row or column of icons / names / IDs, etc.
 * */
#pragma once

#include "backend/Pipeline.h"

#include <glibmm/refptr.h>

#include <gtkmm/window.h>

#include <gtkmm/listbox.h>
#include <gtkmm/label.h>

#include <giomm/liststore.h>

class PipelineWidgetHelper{
    protected:
        PipelineWidgetHelper();
    public:
        static Gtk::Widget* createPipelineEntry(const std::shared_ptr<Pipeline> item);
};

class PipelineSelector{
    public:
        static PipelineSelector* create(Gtk::Window* main_window);
        static PipelineSelector* getInstance() { return s_instance; };

        Gtk::Widget* the() { return &m_listbox; };

        void createPipeline();
        static void addPipeline(Glib::RefPtr<Pipeline>);

    protected:
        PipelineSelector(Gtk::Window* main_window);

    private:
        static PipelineSelector* s_instance;

        Gtk::Window* m_main_window;
        Gtk::ListBox m_listbox;
};
