/*  This view shows the available pipelines at the moment
 *  in the form of a row or column of icons / names / IDs, etc.
 * */
#pragma once

#include "MainWindow.h"
#include "backend/Pipeline.h"
#include "backend/PipelineListModel.h"

#include <glibmm/refptr.h>

#include <gtkmm/treeview.h>

class PipelineSelector{
    public:
        static PipelineSelector* create(MainWindow* main_window);
        static PipelineSelector* getInstance() { return s_instance; };

        static Gtk::TreeModel::Path currentPath();
        static Pipeline* currentPipeline();

        Gtk::Widget& the() { return m_list; };

    protected:
        PipelineSelector(MainWindow* main_window);

    private:
        static PipelineSelector* s_instance;

        MainWindow* m_main_window;

        Glib::RefPtr<PipelineListModel> m_model;
        Gtk::TreeView m_list;
};
