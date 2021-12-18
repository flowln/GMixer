#include "signals/Pipelines.h"
#include "signals/Elements.h"

sigc::signal<void(Pipeline*)>& Signals::pipeline_selected()
{
    static sigc::signal<void(Pipeline*)> signal_pipeline_selected = {};
    return signal_pipeline_selected;
}

sigc::signal<void(Gtk::TreeModel::Path)>& Signals::pipeline_added()
{
    static sigc::signal<void(Gtk::TreeModel::Path)> signal_pipeline_added = {};
    return signal_pipeline_added;
}

sigc::signal<void(Glib::UStringView)>& Signals::element_add()
{
    static sigc::signal<void(Glib::UStringView)> signal_element_add = {};
    return signal_element_add;
}

