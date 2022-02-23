#pragma once

#include "client/gtk/ElementListModel.h"

#include <gtkmm/box.h>
#include <gtkmm/expander.h>
#include <gtkmm/listbox.h>
#include <gtkmm/liststore.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stack.h>
#include <gtkmm/treeview.h>

// Forward-declaration
class ElementRecord;
enum class ElementType;
class MainWindow;

/* High-level widget with a tab for each type of element available. */
class ElementSelector : public Gtk::Notebook {
   public:
    static ElementSelector* create(MainWindow*);
    ElementSelector(Gtk::Window*);

    std::string getSelectedElement();

   private:
    Gtk::Window* m_parent;
};

/* A scrollable list of Gst elements */
class ElementList : public Gtk::ScrolledWindow {
   public:
    ElementList(ElementType type);

    void populate() { m_model->populate(); };
    Gtk::TreeSelection& getSelection() const { return *m_view->get_selection(); };

    // Utility methods
    inline int append_column(const std::string& title, const Gtk::TreeModelColumn<std::string>& model_column)
    {
        return m_view->append_column(title, model_column);
    };

    inline void setSearchable(int index_column) { m_view->get_column(index_column)->set_sort_column(index_column); };

    inline void setAllSearchable()
    {
        int k = m_view->get_n_columns();
        for (int i = 0; i < k; i++)
            setSearchable(i);
    };

    inline ElementRecord& getModelRecord() const;

   private:
    std::unique_ptr<Gtk::TreeView> m_view;
    std::unique_ptr<ElementListModel> m_model;
};

/* Page of information regarding the selected element. */
class SelectedInfoPanel : public Gtk::Stack {
   public:
    SelectedInfoPanel(ElementList*);

    std::string getSelectedPluginName() const;
    std::string getSelectedElementName() const;

    void selectionChanged();
    void deselect();

   private:
    /* Provides information that can be shown or hidden by the user. */
    class OptionalInfo : public Gtk::Expander {
       public:
        OptionalInfo(std::string&& title);
        void append(Gtk::Widget* widget) { m_box->append(*widget); }

        template <typename... T>
        void appendMany(Gtk::Label* widg, T... widgs)
        {
            widg->set_xalign(0);

            append(widg);
            appendMany(widgs...);
        }

        void appendMany() const {}

       private:
        Gtk::Box* m_box;
    };

    ElementList* m_list;

    Gtk::ListBox* m_selected_info_box;

    OptionalInfo m_element_info{ "Element Metadata" };
    Gtk::Label m_name, m_description, m_author;

    OptionalInfo m_plugin_info{ "Plugin Metadata" };
    Gtk::Label m_plugin_name, m_plugin_description, m_plugin_version, m_plugin_license;

    OptionalInfo m_pads_info{ "Pads Information" };
    Gtk::Label m_src_num, m_src_caps, m_sink_num, m_sink_caps;

    Gtk::Label m_not_selected;
};
