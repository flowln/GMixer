#include "client/common/PipelineFactory.h"
#include "ProjectConfiguration.h"

#ifdef GTK_ENABLED
#include "client/GtkClient.h"
#include "client/gtk/PipelineListModel.h"
#include "client/gtk/PipelineSelector.h"
#endif

#include "signals/Pipelines.h"

PipelineFactory* PipelineFactory::create()
{
    return new PipelineFactory();
}

PipelineFactory::PipelineFactory() : m_pipelines(Client::instance()->pipelineStorageAgent()) {}

void PipelineFactory::createEmptyPipeline(std::string const& name)
{
    auto pipeline = Pipeline::create(name);
    m_pipelines->addPipeline(pipeline);
}

void PipelineFactory::createPipelineFromData(std::shared_ptr<FileUtils::file_info> data)
{
    auto pipeline = Pipeline::createFromString(data->command);
    m_pipelines->addPipeline(data->name, pipeline);

    // FIXME: Remove dependency from GTK
#ifdef GTK_ENABLED
    if (dynamic_cast<GtkClient*>(Client::instance())) {
        // PipelineSelector::getInstance()->the().get_selection()->select(added_path);
        // Signals::pipeline_selected().emit(pipeline);
    }
#endif
}
