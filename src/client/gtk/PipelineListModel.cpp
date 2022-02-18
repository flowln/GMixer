#include "PipelineListModel.h"
#include "signals/Pipelines.h"

PipelineRecord::PipelineRecord()
{
    add(m_pipeline_name);
    add(m_pipeline);
}

PipelineListModel::PipelineListModel()
{
    m_store = Gtk::ListStore::create(m_records);
}

PipelineListModel* PipelineListModel::create()
{
    return new PipelineListModel();
}

unsigned int PipelineListModel::numEntries() const
{
    return m_iterators.size();
}

std::weak_ptr<Pipeline> PipelineListModel::getPipeline(unsigned int index)
{
    return (std::shared_ptr<Pipeline>)((*m_iterators[index])[getRecord().m_pipeline]);
}

std::weak_ptr<Pipeline> PipelineListModel::getPipeline(Gtk::TreePath path)
{
    return (std::shared_ptr<Pipeline>)((*m_store->get_iter(path))[getRecord().m_pipeline]);
}

void PipelineListModel::addPipeline(Pipeline* pipeline)
{
    addPipeline(pipeline->getName(), pipeline);
}

void PipelineListModel::addPipeline(std::string&& name, Pipeline* pipeline)
{
    auto iter = m_store->append();
    m_iterators.push_back(iter);

    auto& records = m_records;

    (*iter)[records.m_pipeline_name] = name;
    (*iter)[records.m_pipeline]      = std::shared_ptr<Pipeline>(pipeline);

    signal_row_added(iter);
}
