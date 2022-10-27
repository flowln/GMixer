/*  This view shows the available pipelines at the moment
 *  in the form of a row or column of icons / names / IDs, etc.
 * */
#pragma once

#include "client/GtkClient.h"

#include <glibmm/refptr.h>

#include <gtkmm/treeview.h>

// Forward-declaration
class MainWindow;
class Pipeline;
class PipelineListModel;

class PipelineSelector final : public Gtk::TreeView {
   public:
    static PipelineSelector* create(MainWindow* main_window);

    Gtk::TreeModel::Path currentPath();
    Pipeline* currentPipeline();

   private:
    PipelineSelector(MainWindow* main_window);

    static GtkClient* s_client;

    MainWindow* m_main_window;
};
