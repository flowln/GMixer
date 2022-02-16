#pragma once

#include "gstreamer/Pipeline.h"

#include <gtkmm/liststore.h>

// FIXME: De-GTK-fy(?) this

class PipelineRecord : public Gtk::TreeModelColumnRecord{
    public:
        PipelineRecord();

        Gtk::TreeModelColumn<Glib::ustring> m_pipeline_name; 
        Gtk::TreeModelColumn<Glib::RefPtr<Pipeline>> m_pipeline; 
};

class PipelineListModel : public Glib::Object {
    public:
        static std::unique_ptr<PipelineListModel> create();

        static Gtk::TreePath addPipeline(Glib::RefPtr<Pipeline> pipeline);
        static Gtk::TreePath addPipeline(Glib::ustring&& name, Glib::RefPtr<Pipeline> pipeline);
        static Pipeline* getPipeline(const Gtk::TreeModel::Path&);

        Glib::RefPtr<Gtk::ListStore> getModel() const { return m_store; }; 
        PipelineRecord& getRecord() { return m_records; };

    protected:
        PipelineListModel();

    private:
        static PipelineListModel* s_instance;

        Glib::RefPtr<Gtk::ListStore> m_store;
        PipelineRecord m_records;
};
