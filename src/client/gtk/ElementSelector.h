#pragma once

#include <gtkmm/box.h>
#include <gtkmm/expander.h>
#include <gtkmm/listbox.h>
#include <gtkmm/notebook.h>
#include <gtkmm/stack.h>

// Forward-declaration
class ElementRecord;
class MainWindow;
class ElementList;

/* High-level widget with a tab for each type of element available. */
class ElementSelector : public Gtk::Notebook {
   public:
    static ElementSelector* create(MainWindow*);

    ElementSelector(Gtk::Window*);
   private:
    Gtk::Window* m_parent;
};

/* Page of information regarding the selected element. */
class SelectedInfoPanel : public Gtk::Stack {
   public:
    SelectedInfoPanel(ElementList*);

    void selectionChanged(guint);
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

    ElementList* m_element_list;

    Gtk::ListBox* m_selected_info_box;

    OptionalInfo m_element_info{ "Element Metadata" };
    Gtk::Label m_name, m_description, m_author;

    OptionalInfo m_plugin_info{ "Plugin Metadata" };
    Gtk::Label m_plugin_name, m_plugin_description, m_plugin_version, m_plugin_license;

    OptionalInfo m_pads_info{ "Pads Information" };
    Gtk::Label m_src_num, m_src_caps, m_sink_num, m_sink_caps;

    Gtk::Label m_not_selected;
};
