#include "backend/PipelineCreation.h"
#include "views/PipelineSelector.h"
#include "HeaderBar.h"

PipelineSelector* PipelineSelector::s_instance = nullptr;

PipelineWidgetHelper::PipelineWidgetHelper(){}

Gtk::Widget* PipelineWidgetHelper::createPipelineEntry(const std::shared_ptr<Pipeline> item)
{
    if(!item)
        return Gtk::make_managed<Gtk::Label>("nullptr");

    auto w = Gtk::make_managed<Gtk::Label>(item->getName());
    w->set_ellipsize(Pango::EllipsizeMode::END);
    return w;
}

PipelineSelector* PipelineSelector::create(Gtk::Window* main_window)
{
    if(s_instance)
        return s_instance;

    s_instance = new PipelineSelector(main_window);
    return s_instance;
}

PipelineSelector::PipelineSelector(Gtk::Window* main_window)
    : m_main_window(main_window)

{
    PipelineFactory::setMainWindow(main_window);

    m_listbox.signal_row_selected().connect(
        [](Gtk::ListBoxRow* row){
            auto label = dynamic_cast<Gtk::Label*>(row->get_child());
            if(!label)
                return;
            HeaderBar::setTitleText(label->get_text());
        });
}

void PipelineSelector::addPipeline(Glib::RefPtr<Pipeline> pipeline)
{
    getInstance()->m_listbox.append(*PipelineWidgetHelper::createPipelineEntry(pipeline));
}
