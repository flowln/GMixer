#include "backend/PipelineCreation.h"
#include "views/PipelineSelector.h"
#include "HeaderBar.h"

PipelineSelector* PipelineSelector::s_instance = nullptr;

PipelineSelector* PipelineSelector::create(Gtk::Window* main_window)
{
    if(s_instance)
        return s_instance;

    s_instance = new PipelineSelector(main_window);
    return s_instance;
}

PipelineSelector::PipelineSelector(Gtk::Window* main_window)
    : m_model(PipelineListModel::create())
    , m_main_window(main_window)
{
    PipelineFactory::setMainWindow(main_window);
    PipelineListModel::create();
    
    m_list = Gtk::make_managed<Gtk::TreeView>(m_model->getModel());
    m_list->append_column("", m_model->getRecord().m_pipeline_name);

    m_list->get_column(0)->set_fixed_width(m_list->get_width());
    m_list->set_headers_visible(false);
    m_list->set_reorderable(true);
    m_list->set_activate_on_single_click(true);

    auto renderer = dynamic_cast<Gtk::CellRendererText*>(m_list->get_column_cell_renderer(0));
    renderer->property_ellipsize().set_value(Pango::EllipsizeMode::END);
    renderer->set_alignment(0.5, 0);

    auto& model = *m_model;

    m_list->signal_row_activated().connect(
        [&model](Gtk::TreePath path, Gtk::TreeViewColumn* column){
            HeaderBar::setTitleText((*model.getModel()->get_iter(path))[model.getRecord().m_pipeline_name]);
        });
}

