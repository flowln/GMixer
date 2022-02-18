#include "PipelineListModel.h"
#include "signals/Pipelines.h"

PipelineListModel* PipelineListModel::s_instance = nullptr;

PipelineRecord::PipelineRecord()
{
    add(m_pipeline_name);
    add(m_pipeline);
}

std::unique_ptr<PipelineListModel> PipelineListModel::create()
{
    if (s_instance)
        return nullptr;

    s_instance = new PipelineListModel();
    return std::unique_ptr<PipelineListModel>(s_instance);
}

Pipeline* PipelineListModel::getPipeline(const Gtk::TreeModel::Path& path)
{
    if (!s_instance)
        return nullptr;

    auto pipeline =
        (Glib::RefPtr<Pipeline>)(*s_instance->getModel()->get_iter(path))[s_instance->getRecord().m_pipeline];

    return pipeline.get();
}

PipelineListModel::PipelineListModel()
{
    m_store = Gtk::ListStore::create(m_records);
}

Gtk::TreePath PipelineListModel::addPipeline(Glib::RefPtr<Pipeline> pipeline)
{
    return addPipeline(pipeline->getName(), pipeline);
}

Gtk::TreePath PipelineListModel::addPipeline(Glib::ustring&& name, Glib::RefPtr<Pipeline> pipeline)
{
    // if(!s_instance)
    //     return {};

    auto iter     = s_instance->m_store->append();
    auto& records = s_instance->m_records;

    (*iter)[records.m_pipeline_name] = name;
    (*iter)[records.m_pipeline]      = pipeline;
    return s_instance->getModel()->get_path(iter);

    // Signals::pipeline_added().emit(s_instance->getModel()->get_path(iter));
}
