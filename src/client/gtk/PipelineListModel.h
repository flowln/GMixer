#pragma once

#include "client/common/Storage.h"

#include <gtkmm/liststore.h>

class PipelineRecord : public Gtk::TreeModelColumnRecord {
   public:
    PipelineRecord();

    Gtk::TreeModelColumn<std::string> m_pipeline_name;
    Gtk::TreeModelColumn<std::shared_ptr<Pipeline>> m_pipeline;
};

class PipelineListModel : public PipelineStorage {
   public:
    static PipelineListModel* create();

    virtual unsigned int numEntries() const override;
    virtual std::weak_ptr<Pipeline> getPipeline(unsigned int) override;
    virtual void addPipeline(Pipeline*) override;
    virtual void addPipeline(std::string&&, Pipeline*) override;

    std::weak_ptr<Pipeline> getPipeline(Gtk::TreePath);
    std::shared_ptr<Gtk::ListStore> getModel() const { return m_store; };
    PipelineRecord& getRecord() { return m_records; };

    sigc::signal<void(Gtk::TreeModel::iterator&)> signal_row_added;

   private:
    PipelineListModel();

    std::shared_ptr<Gtk::ListStore> m_store;
    // FIXME: Handle case of reordering
    std::vector<Gtk::TreeModel::iterator> m_iterators;
    PipelineRecord m_records;
};
