#include "client/common/PipelineFactory.h"
#include "client/gtk/PipelineSelector.h"
#include "client/GtkClient.h"

#include "gstreamer/PipelineListModel.h"

#include "signals/Pipelines.h"

Client* PipelineFactory::s_client = nullptr;

void PipelineFactory::createEmptyPipeline(Glib::ustring const& name)
{
    auto pipeline = Pipeline::create(name);
    PipelineListModel::addPipeline(Glib::make_refptr_for_instance(pipeline));
}

void PipelineFactory::createPipelineFromData(std::shared_ptr<FileUtils::file_info> data)
{
    auto pipeline = Pipeline::createFromString(data->command);
    auto added_path = PipelineListModel::addPipeline(data->name, Glib::make_refptr_for_instance(pipeline));

    if(dynamic_cast<GtkClient*>(s_client)){
        PipelineSelector::getInstance()->the().get_selection()->select(added_path);
        Signals::pipeline_selected().emit(PipelineSelector::currentPipeline()); 
    }

}   