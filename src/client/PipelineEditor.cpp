#include "gstreamer/Element.h"
#include "gstreamer/ElementUtils.h"
#include "gstreamer/Pipeline.h"
#include "signals/Pipelines.h"
#include "signals/Elements.h"
#include "signals/Graph.h"
#include "client/PipelineEditor.h"
#include "client/graph/ElementNode.h"
#include "client/graph/GraphViewer.h"

#include <gtkmm/entry.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>

using namespace GMixer;

PipelineGraph::PipelineGraph(PipelineEditor* parent)
    : Gtk::Box(Gtk::Orientation::VERTICAL)
    , m_editor(parent)
{
    set_parent(*parent);
    //set_size_request(200, 50);

    m_bar = Gtk::make_managed<Gtk::ActionBar>();
    m_viewer = Gtk::make_managed<GraphViewer>(this);

    auto mode_select = Gtk::make_managed<Gtk::ToggleButton>("Select");
    mode_select->set_image_from_icon_name("edit-select");
    auto mode_move = Gtk::make_managed<Gtk::ToggleButton>("Move");
    mode_move->set_image_from_icon_name("fitbest");
    auto mode_cut = Gtk::make_managed<Gtk::ToggleButton>("Cut");
    mode_cut->set_image_from_icon_name("edit-cut-symbolic");

    mode_select->set_active(true);
    mode_move->set_group(*mode_select);
    mode_cut->set_group(*mode_select);

    mode_select->signal_clicked().connect([this]{ m_mode = OperationMode::MODE_SELECT; });
    mode_move->signal_clicked().connect([this]{ m_mode= OperationMode::MODE_MOVE; });
    mode_cut->signal_clicked().connect([this]{ m_mode= OperationMode::MODE_CUT; });

    auto tip = Gtk::make_managed<Gtk::Label>("To add an element to the pipeline, select it in the list below and click the 'Add' button.");
    tip->set_ellipsize(Pango::EllipsizeMode::END);

    m_bar->pack_start(*mode_select);
    m_bar->pack_start(*mode_move);
    m_bar->pack_start(*mode_cut);
    m_bar->pack_end(*tip);

    append(*m_viewer);
    append(*m_bar);

}

void PipelineGraph::addElement(Element* element)
{
    m_viewer->addNode(ElementNode::create(m_viewer, element, 0, 0));
}

ElementPropertyEditor::ElementPropertyEditor(Gtk::Widget& parent)
    : Gtk::ScrolledWindow()
{
    set_parent(parent);
    set_size_request(140, -1);
    m_properties = Gtk::make_managed<Gtk::ListBox>();
    m_properties->set_selection_mode(Gtk::SelectionMode::NONE);
    set_child(*m_properties);

    Signals::node_selected().connect(sigc::mem_fun(*this, &ElementPropertyEditor::hook));
}
void ElementPropertyEditor::hook(Node* node)
{
    if(m_hooked_node == node)
        return;

    if(m_hooked_properties)
        m_hooked_properties->destroy();
    m_hooked_node = node;
    
    Gtk::Widget* widg;
    while((widg = m_properties->get_row_at_index(0))){
        m_properties->remove(*widg);
    }

    if(!node)
        return;
    
    m_properties->append(*Gtk::make_managed<Gtk::Label>(Glib::ustring::sprintf("Properties of %s", node->getName().c_str())));
    try{
        m_hooked_properties = node->getProperties();
        for(auto prop : *m_hooked_properties){
            m_properties->append(*createWidget(prop));
        }
    } catch(const std::exception& ex){
        printf("[ERROR] Could not get properties of %s:\n%s\n", node->getName().c_str(), ex.what());
    }
}
Gtk::Box* ElementPropertyEditor::createWidget(GMixer::Property* prop)
{
    auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    box->set_spacing(5);

    auto name = Gtk::make_managed<Gtk::Label>(prop->getName());
    name->set_tooltip_text(prop->getField("desc"));

    auto entry = Gtk::make_managed<Gtk::Entry>();
    auto def = prop->getField("default");
    auto def_type = prop->getType("default");
    entry->set_placeholder_text(def);
    entry->set_tooltip_text(Glib::ustring::sprintf("Format: %s\t\tDefault: %s", g_type_name(def_type), def)); 

    auto confirm_button = Gtk::make_managed<Gtk::Button>("Confirm");
    confirm_button->set_sensitive(false);

    entry->signal_changed().connect(
        [confirm_button, entry](){ 
            confirm_button->set_sensitive(entry->get_text_length() != 0); 
        });
    confirm_button->signal_clicked().connect(
        [entry, prop]{
            prop->updateField("value", entry->get_text());
        });

    box->append(*name);
    box->append(*entry);
    box->append(*confirm_button);

    return box;
}

PipelineEditor::PipelineEditor(Gtk::Widget& parent, Pipeline* pipeline)
    : Gtk::Paned()
    , m_pipeline(pipeline)
{
    m_graph = Gtk::make_managed<PipelineGraph>(this);
    m_properties = Gtk::make_managed<ElementPropertyEditor>(*this);

    set_start_child(*m_graph);
    set_end_child(*m_properties);

    Signals::pipeline_selected().connect(
        [this](Pipeline* selected){
            m_is_selected = selected == m_pipeline;
        });

    Signals::element_add().connect(
        [this](Glib::UStringView name){
            if(m_is_selected){
                m_graph->addElement(m_pipeline->createElement(name.c_str()));
            }
        });

    // Check if pipeline is not empty, and populate the graph if that's the case
    if(!m_pipeline->isEmpty()){
        auto iterator = m_pipeline->getElementsSorted();
        GValue item = G_VALUE_INIT;
        bool done = FALSE;

        while(!done){
            switch (gst_iterator_next (iterator.get(), &item)) {
            case GST_ITERATOR_OK:
                m_graph->addElement(new Element(GST_ELEMENT( g_value_get_object(&item))));
                g_value_reset (&item);
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync (iterator.get());
                break;
            case GST_ITERATOR_ERROR:
                done = TRUE;
                break;
            case GST_ITERATOR_DONE:
                done = TRUE;
                break;
            }
        }

        g_value_unset(&item);
    }
}
