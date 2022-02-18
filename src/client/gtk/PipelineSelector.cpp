#include "gstreamer/Pipeline.h"

#include "signals/Pipelines.h"

#include "client/gtk/HeaderBar.h"
#include "client/gtk/MainWindow.h"
#include "client/gtk/PipelineEditor.h"
#include "client/gtk/PipelineListModel.h"
#include "client/gtk/PipelineSelector.h"

GtkClient* PipelineSelector::s_client = nullptr;

void PipelineSelector::setClient(Client* client)
{
    if ((s_client = static_cast<GtkClient*>(client)))
        return;
    else if (client)
        client->loggingAgent()->sendError("PipelineSelector's client is not a GTK client!");
}

PipelineSelector* PipelineSelector::create(MainWindow* main_window)
{
    return new PipelineSelector(main_window);
}

Gtk::TreeModel::Path PipelineSelector::currentPath()
{
    return s_client->pipelineStorageAgent()->getModel()->get_path(get_selection()->get_selected());
}

Pipeline* PipelineSelector::currentPipeline()
{
    return s_client->pipelineStorageAgent()->getPipeline(currentPath()).lock().get();
}

PipelineSelector::PipelineSelector(MainWindow* main_window) : m_main_window(main_window)
{
    set_size_request(100, -1);
    set_vexpand(true);

    auto storage = s_client->pipelineStorageAgent();

    set_model(storage->getModel());
    // FIXME: Add CSS to center column header
    append_column("Pipelines", storage->getRecord().m_pipeline_name);

    get_column(0)->set_fixed_width(get_width());
    set_headers_visible(true);
    set_activate_on_single_click(true);

    auto renderer = static_cast<Gtk::CellRendererText*>(get_column_cell_renderer(0));
    renderer->property_ellipsize().set_value(Pango::EllipsizeMode::END);
    renderer->set_alignment(0.5, 0);

    storage->signal_row_added.connect([&](Gtk::TreeIter<Gtk::TreeRow>& iter) {
        get_selection()->select(iter);
        Signals::pipeline_selected().emit(currentPipeline());
    });

    signal_row_activated().connect([&](const Gtk::TreePath& path, Gtk::TreeViewColumn* const& column) {
        Signals::pipeline_selected().emit(currentPipeline());
    });
}
