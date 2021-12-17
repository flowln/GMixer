#include "backend/PipelineIO.h"
#include "views/PipelineEditor.h"
#include "views/PipelineSelector.h"
#include "HeaderBar.h"

PipelineSelector* PipelineSelector::s_instance = nullptr;
sigc::signal<void(Pipeline*)> PipelineSelector::signal_pipeline_selected = {};

PipelineSelector* PipelineSelector::create(MainWindow* main_window)
{
    if(s_instance)
        return s_instance;

    s_instance = new PipelineSelector(main_window);
    return s_instance;
}

Gtk::TreeModel::Path PipelineSelector::currentPath()
{
    if(!s_instance)
        return {};

    return s_instance->m_model->getModel()->get_path(s_instance->m_list.get_selection()->get_selected());
}

Pipeline* PipelineSelector::currentPipeline()
{
    return PipelineListModel::getPipeline(currentPath());
}

PipelineSelector::PipelineSelector(MainWindow* main_window)
    : m_main_window(main_window)
    , m_model(PipelineListModel::create())
{
    PipelineFactory::setMainWindow(main_window);
    PipelineListModel::create();
    
    m_list.set_model(m_model->getModel());
    m_list.append_column("", m_model->getRecord().m_pipeline_name);

    m_list.get_column(0)->set_fixed_width(m_list.get_width());
    m_list.set_headers_visible(false);
    m_list.set_reorderable(true);
    m_list.set_activate_on_single_click(true);

    auto renderer = dynamic_cast<Gtk::CellRendererText*>(m_list.get_column_cell_renderer(0));
    renderer->property_ellipsize().set_value(Pango::EllipsizeMode::END);
    renderer->set_alignment(0.5, 0);

    m_list.signal_row_activated().connect([](const Gtk::TreePath& path, Gtk::TreeViewColumn* const& column){ signal_pipeline_selected.emit(currentPipeline()); });
}

