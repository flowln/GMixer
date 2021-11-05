/*  This view is responsible for showing the available elements
 *  for the user to include in the pipeline.
 * */
#pragma once

#include <cstdarg>
#include <iostream>

#include <gtkmm/box.h>
#include <gtkmm/expander.h>
#include <gtkmm/liststore.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stack.h>
#include <gtkmm/treeview.h>

// Forward-declaration
class ElementListModel;
class ElementRecord;
enum class ElementType;

class ElementList : public Gtk::ScrolledWindow{
    public:
        ElementList(const ElementType& type);

        Gtk::TreeView& operator->() const { return *m_view; };
        ElementListModel& getModel() const { return *m_model; };
        Gtk::TreeSelection& getSelection() const { return *m_view->get_selection(); };

        // Utility shortcuts
        inline int append_column(const Glib::ustring& title, const Gtk::TreeModelColumn<Glib::ustring>& model_column) 
            { return m_view->append_column(title, model_column); };
        
        inline void setSearchable(int index_column) 
            { m_view->get_column(index_column)->set_sort_column(index_column); }; 
        
        inline void setAllSearchable() 
            { int k = m_view->get_n_columns(); for(int i = 0; i < k; i++) setSearchable(i); };

        inline ElementRecord& getModelRecord() const;
    private:
        std::unique_ptr<Gtk::TreeView> m_view;
        std::unique_ptr<ElementListModel> m_model;    
};

class OptionalInfo : public Gtk::Expander {
    public:
        OptionalInfo(const Glib::ustring& title) : Gtk::Expander()
        { 
            set_label(title);
            m_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
            m_box->set_halign(Gtk::Align::START);
            set_child(*m_box); 
        }
        void append(Gtk::Widget* widget){ m_box->append(*widget); }

        template<typename... T>
        void appendMany(Gtk::Widget* widg, T... widgs);
        void appendMany() const{}

    private:
        Gtk::Box* m_box;
};

class SelectedInfoPanel : public Gtk::Stack{
    public:
        SelectedInfoPanel(ElementList*);

        void selectionChanged();
        void deselect();
    private:
        ElementList* m_list;

        Gtk::Box* m_selected_info_box;

        OptionalInfo m_element_info {"Element Metadata"};
        Gtk::Label m_name, m_description, m_author;

        OptionalInfo m_plugin_info {"Plugin Metadata"};
        Gtk::Label m_plugin_name, m_plugin_description, m_plugin_version, m_plugin_license;

        Gtk::Label m_not_selected;
};

class ElementSelector {
    public:
        ElementSelector(Gtk::Window* m_main_window);

        Gtk::Widget* the() const { return m_notebook; };
    private: 
        Gtk::Window* m_main_window;
        Gtk::Notebook* m_notebook = { nullptr };
};
