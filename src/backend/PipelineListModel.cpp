#include "PipelineListModel.h"

PipelineListModel* PipelineListModel::s_instance = nullptr;

PipelineRecord::PipelineRecord()
{
    add(m_pipeline_name);
    add(m_pipeline);
}

std::unique_ptr<PipelineListModel> PipelineListModel::create()
{
    if(s_instance)
        return nullptr;

    s_instance = new PipelineListModel();
    return std::unique_ptr<PipelineListModel>(s_instance);
}

PipelineListModel::PipelineListModel()
{
    m_store = Gtk::ListStore::create(m_records);
}

void PipelineListModel::addPipeline(Glib::RefPtr<Pipeline> pipeline)
{
    if(!s_instance)
        return;

    auto iter = s_instance->m_store->append();
    auto& records = s_instance->m_records;

    (*iter)[records.m_pipeline_name] = *pipeline->getNameReference();
    (*iter)[records.m_pipeline] = pipeline; 
}
