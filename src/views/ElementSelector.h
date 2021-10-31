/*  This view is responsible for showing the available elements
 *  for the user to include in the pipeline.
 * */
#pragma once

#include "backend/ElementListModel.h"

#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>

// Forward-declaration
class ElementListModel;

class ElementList : public Gtk::ScrolledWindow{
    public:
        ElementList(const ElementType& type);

        Gtk::TreeView& operator->() const { return *m_view; };
        ElementListModel& getModel() const { return *m_model; };

        // Utility shortcuts
        inline void append_column(const Glib::ustring& title, const Gtk::TreeModelColumn<Glib::ustring>& model_column) 
            { m_view->append_column(title, model_column); };
        inline ElementRecord& getModelRecord() const { return m_model->getRecord(); };

    private:
        std::unique_ptr<Gtk::TreeView> m_view;
        std::unique_ptr<ElementListModel> m_model;
        
};

class ElementSelector {
    public:
        ElementSelector(Gtk::Window* m_main_window);

        Gtk::Notebook* the() const { return m_notebook; };
    private: 
        Gtk::Window* m_main_window;

        Gtk::Notebook* m_notebook = { nullptr };
};
