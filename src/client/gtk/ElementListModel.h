#pragma once

#include "client/common/Storage.h"

#include <gst/gst.h>
#include <gtkmm/liststore.h>

class ElementRecord : public Gtk::TreeModelColumnRecord {
   public:
    ElementRecord();

    Gtk::TreeModelColumn<std::string> m_element_name;
    Gtk::TreeModelColumn<std::string> m_plugin;
    Gtk::TreeModelColumn<std::string> m_package;
};

enum class ElementType { SOURCE, FILTER, SINK };

class ElementListModel : public ElementStorage {
   public:
    static ElementListModel* create(ElementType);

    virtual unsigned int numEntries() const override { return m_entries; };
    virtual void populate(bool blocking = false) override;

    Glib::RefPtr<Gtk::ListStore> getModel() const { return m_store; };
    ElementRecord& getRecord() { return m_records; };

   private:
    ElementListModel(ElementType type);
    void addElement(GstPlugin* plugin, GstPluginFeature* element);
    void populateInternal();

    bool isPopulated{ false };
    unsigned int m_entries = 0;

    ElementType m_type;

    Glib::RefPtr<Gtk::ListStore> m_store;
    ElementRecord m_records;
};
