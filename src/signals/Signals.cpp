#include "signals/Elements.h"
#include "signals/Graph.h"
#include "signals/Pipelines.h"

sigc::signal<void(Pipeline*)>& Signals::pipeline_selected()
{
    static sigc::signal<void(Pipeline*)> signal_pipeline_selected = {};
    return signal_pipeline_selected;
}
/*
sigc::signal<void(Gtk::TreeModel::Path)>& Signals::pipeline_added()
{
    static sigc::signal<void(Gtk::TreeModel::Path)> signal_pipeline_added = {};
    return signal_pipeline_added;
}
*/
sigc::signal<void(std::string)>& Signals::element_add()
{
    static sigc::signal<void(std::string)> signal_element_add = {};
    return signal_element_add;
}

sigc::signal<void(Node*)> Signals::node_selected()
{
    static sigc::signal<void(Node*)> signal_node_selected = {};
    return signal_node_selected;
}
