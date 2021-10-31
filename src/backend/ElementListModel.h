#pragma once

#include <glibmm/object.h>
#include <gtkmm/liststore.h>
#include <gst/gstelement.h>

class ElementRecord : public Gtk::TreeModelColumnRecord{
    public:
        ElementRecord();

        Gtk::TreeModelColumn<Glib::ustring> m_element_name; 
        Gtk::TreeModelColumn<Glib::ustring> m_plugin; 
        Gtk::TreeModelColumn<Glib::ustring> m_package;
};

enum class ElementType{
    SOURCE, FILTER, SINK
};

class ElementListModel{
    public:
        ElementListModel(ElementType type);
        void populate();

        Glib::RefPtr<Gtk::ListStore> getModel() const { return m_store; };
        ElementRecord& getRecord() { return m_records; };

    private:
        void addElement(GstPlugin* plugin, GstPluginFeature* element);
        void listElements();

        ElementType m_type;

        Glib::RefPtr<Gtk::ListStore> m_store;
        ElementRecord m_records;
};
