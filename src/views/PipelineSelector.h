/*  This view shows the available pipelines at the moment
 *  in the form of a row or column of icons / names / IDs, etc.
 * */
#pragma once

#include "backend/Pipeline.h"
#include "backend/PipelineListModel.h"

#include <glibmm/refptr.h>

#include <gtkmm/window.h>

#include <gtkmm/treeview.h>

class PipelineSelector{
    public:
        static PipelineSelector* create(Gtk::Window* main_window);
        static PipelineSelector* getInstance() { return s_instance; };

        Gtk::Widget* the() { return m_list; };

        void createPipeline();
    protected:
        PipelineSelector(Gtk::Window* main_window);

    private:
        static PipelineSelector* s_instance;

        Glib::RefPtr<PipelineListModel> m_model;

        Gtk::Window* m_main_window;
        Gtk::TreeView* m_list;
};
