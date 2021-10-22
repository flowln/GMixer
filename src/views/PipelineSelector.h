/*  This view shows the available pipelines at the moment
 *  in the form of a row or column of icons / names / IDs, etc.
 * */
#pragma once

#include "backend/Pipeline.h"

#include <glibmm/refptr.h>

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
        PipelineSelector();

        Gtk::Widget* the() { return &m_listbox; };
    private:
        Gtk::ListBox m_listbox;
};
