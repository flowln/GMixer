#include "PipelineSelector.h"

PipelineWidgetHelper::PipelineWidgetHelper(){}

Gtk::Widget* PipelineWidgetHelper::createPipelineEntry(const std::shared_ptr<Pipeline> item)
{
    if(!item)
        return nullptr;

    auto w = Gtk::make_managed<Gtk::Label>(item->getName());
    return w;
}

PipelineSelector::PipelineSelector()
{
    auto p1 = Pipeline::create("joaquim");
    auto p2 = Pipeline::create("maria");
    m_listbox.append(*PipelineWidgetHelper::createPipelineEntry(p1));
    m_listbox.append(*PipelineWidgetHelper::createPipelineEntry(p2));
}

