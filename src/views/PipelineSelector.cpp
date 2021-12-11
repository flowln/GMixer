#include "views/PipelineSelector.h"
#include "HeaderBar.h"

#include <sigc++/functors/mem_fun.h>

PipelineWidgetHelper::PipelineWidgetHelper(){}

Gtk::Widget* PipelineWidgetHelper::createPipelineEntry(const std::shared_ptr<Pipeline> item)
{
    if(!item)
        return nullptr;

    auto w = Gtk::make_managed<Gtk::Label>(item->getName());
    return w;
}

PipelineSelector::PipelineSelector(Gtk::Window* main_window)
    : m_main_window(main_window)

{
    HeaderBar::setAddButtonCallback(sigc::mem_fun(*this, &PipelineSelector::add_pipeline));

    m_listbox.signal_row_selected().connect(
        [](Gtk::ListBoxRow* row){
            auto label = dynamic_cast<Gtk::Label*>(row->get_child());
            if(!label)
                return;
            HeaderBar::setTitleText(label->get_text());
        });
}

void PipelineSelector::add_pipeline()
{
    static int id = 0;

    //TODO: Add dialog asking for name / icon / other info about pipeline

    auto p = Pipeline::create("Pipeline " + std::to_string(id++));
    m_listbox.append(*PipelineWidgetHelper::createPipelineEntry(p));
}
