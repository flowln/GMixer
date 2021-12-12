#include "backend/PipelineCreation.h"
#include "views/PipelineSelector.h"
#include "HeaderBar.h"

#include <sigc++/functors/mem_fun.h>

PipelineWidgetHelper::PipelineWidgetHelper(){}

Gtk::Widget* PipelineWidgetHelper::createPipelineEntry(const std::shared_ptr<Pipeline> item)
{
    if(!item)
        return Gtk::make_managed<Gtk::Label>("nullptr");

    auto w = Gtk::make_managed<Gtk::Label>(item->getName());
    w->set_ellipsize(Pango::EllipsizeMode::END);
    return w;
}

PipelineSelector::PipelineSelector(Gtk::Window* main_window)
    : m_main_window(main_window)

{
    PipelineFactory::setMainWindow(main_window);
    HeaderBar::setAddButtonCallback(sigc::mem_fun(*this, &PipelineSelector::createPipeline));

    m_listbox.signal_row_selected().connect(
        [](Gtk::ListBoxRow* row){
            auto label = dynamic_cast<Gtk::Label*>(row->get_child());
            if(!label)
                return;
            HeaderBar::setTitleText(label->get_text());
        });
}

void PipelineSelector::createPipeline()
{
    PipelineFactory::createPipeline(sigc::mem_fun(*this, &PipelineSelector::addPipeline));
}

void PipelineSelector::addPipeline(Glib::RefPtr<Pipeline> pipeline)
{
    m_listbox.append(*PipelineWidgetHelper::createPipelineEntry(pipeline));
}
