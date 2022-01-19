#include "gstreamer/Element.h"
#include "gstreamer/ElementUtils.h"
#include "gstreamer/Pipeline.h"
#include "signals/Pipelines.h"
#include "signals/Elements.h"
#include "signals/Graph.h"
#include "client/gtk/PipelineEditor.h"
#include "client/gtk/graph/ElementNode.h"
#include "client/gtk/graph/GraphViewer.h"

#include <gtkmm/entry.h>
#include <gtkmm/eventcontrollerkey.h>
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

    m_mode_select = Gtk::make_managed<Gtk::ToggleButton>("Select");
    m_mode_select->set_image_from_icon_name("edit-select");
    m_mode_select->set_tooltip_text("Select node or link");

    m_mode_move = Gtk::make_managed<Gtk::ToggleButton>("Move");
    m_mode_move->set_image_from_icon_name("fitbest");
    m_mode_move->set_tooltip_text("Move node");

    m_mode_cut = Gtk::make_managed<Gtk::ToggleButton>("Cut");
    m_mode_cut->set_image_from_icon_name("edit-cut-symbolic");
    m_mode_cut->set_tooltip_text("Cut existing links");

    m_mode_select->set_active(true);
    m_mode_move->set_group(*m_mode_select);
    m_mode_cut->set_group(*m_mode_select);

    m_mode_select->signal_clicked().connect([this]{ setMode(OperationMode::MODE_SELECT); });
    m_mode_move->signal_clicked().connect([this]{ setMode(OperationMode::MODE_MOVE); });
    m_mode_cut->signal_clicked().connect([this]{ setMode(OperationMode::MODE_CUT); });

    auto tip = Gtk::make_managed<Gtk::Label>("To add an element to the pipeline, select it in the list below and click the 'Add' button.");
    tip->set_ellipsize(Pango::EllipsizeMode::END);

    m_bar->pack_start(*m_mode_select);
    m_bar->pack_start(*m_mode_move);
    m_bar->pack_start(*m_mode_cut);
    m_bar->pack_end(*tip);

    append(*m_viewer);
    append(*m_bar);

    // Sets up keyboard shortcuts
    // Return to previous mode when key is released
    static OperationMode last_mode;
    
    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(
        [&](guint keyval, guint keycode, Gdk::ModifierType state){
            if(keyval == GDK_KEY_Control_L){
                last_mode = m_mode;
                setMode(OperationMode::MODE_MOVE);
            }
           return true; 
        }, false);
    key_controller->signal_key_released().connect(
        [&](guint keyval, guint keycode, Gdk::ModifierType state){
            if(keyval == GDK_KEY_Control_L){
                setMode(last_mode);
            }
        }, false);

    add_controller(key_controller);
}

void PipelineGraph::addElement(Element* element)
{
    m_viewer->addNode(ElementNode::create(m_viewer, element, 0, 0));
}

void PipelineGraph::setMode(OperationMode new_mode)
{
    static auto move_cursor = Gdk::Cursor::create("move");

    m_mode = new_mode;
    switch(new_mode){
    case OperationMode::MODE_SELECT:
        set_cursor("grab");
        break;
    case OperationMode::MODE_MOVE:
        set_cursor(move_cursor);
        break;
    case OperationMode::MODE_CUT:
        set_cursor();
        break;
    }
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

    m_hooked_node = node;
    
    Gtk::Widget* widg;
    while((widg = m_properties->get_row_at_index(0))){
        m_properties->remove(*widg);
    }

    if(!node)
        return;
    
    m_properties->append(*Gtk::make_managed<Gtk::Label>(Glib::ustring::sprintf("Properties of %s", node->getName().c_str())));
    try{
        auto properties = node->getProperties();
        for(auto prop : *properties){
            m_properties->append(*createWidget(prop));
        }
    } catch(const std::exception& ex){
        printf("[ERROR] Could not get properties of %s:\n%s\n", node->getName().c_str(), ex.what());
    }
}
Gtk::Box* ElementPropertyEditor::createWidget(GMixer::Property* prop)
{
    auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    box->set_spacing(8);

    auto name = Gtk::make_managed<Gtk::Label>();
    name->set_markup(Glib::ustring::sprintf("<i>%s</i>", prop->getName()));
    name->set_tooltip_text(prop->getField("desc"));

    auto default_value = prop->getField("default");
    auto default_type = prop->getType("default");
    auto value = prop->getField("value");

    auto entry = Gtk::make_managed<Gtk::Entry>();
    entry->set_hexpand(true);
    if(!value.empty())
        entry->set_text(value);
    entry->set_placeholder_text(default_value);
    entry->set_tooltip_text(Glib::ustring::sprintf("Format: %s\t\tDefault: %s", g_type_name(default_type), default_value)); 

    auto confirm_button = Gtk::make_managed<Gtk::Button>("Confirm");
    confirm_button->set_sensitive(false);

    entry->signal_changed().connect(
        [confirm_button, entry, prop](){ 
            auto value = prop->getField("value");
            if(value.empty())
                confirm_button->set_sensitive(entry->get_text_length() != 0);
            else
                confirm_button->set_sensitive(std::string(entry->get_text()) != value); 
        });
    confirm_button->signal_clicked().connect(
        [confirm_button, entry, prop]{
            prop->updateField("value", entry->get_text());
            confirm_button->set_sensitive(false);
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
                m_graph->addElement(new Element(name.c_str()));
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
