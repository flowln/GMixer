#include "client/gtk/graph/ElementNode.h"
#include "client/gtk/PipelineEditor.h"
#include "client/gtk/graph/GraphViewer.h"
#include "client/gtk/graph/Pad.h"
#include "gst/gstpadtemplate.h"

#include <gtkmm/entry.h>
#include <gtkmm/label.h>

#include <variant>

ElementNode* ElementNode::create(GraphViewer* parent, Element* element, int x, int y)
{
    auto node = new ElementNode(parent, element, x, y);
    node->updateNode();
    return node;
}

ElementNode* ElementNode::create(GraphViewer* parent, const gchar* element_name, int x, int y)
{
    return ElementNode::create(parent, new Element(element_name), x, y);
}

ElementNode::~ElementNode()
{
    delete m_element;
}

ElementNode::ElementNode(GraphViewer* parent, Element* element, int x, int y)
    : Node(parent, element->getName(), x, y), m_element(element)
{
    // Add always available pads
    auto sources = element->getSources();
    while (sources) {
        addInputPad(new InputPad(this, GST_PAD(sources->data)));
        sources = sources->next;
    }
    auto sinks = element->getSinks();
    while (sinks) {
        addOutputPad(new OutputPad(this, GST_PAD(sinks->data)));
        sinks = sinks->next;
    }

    auto templates = element->getPadTemplates();
    while (templates) {
        auto& templ = templates->data;
        int presence;

        if ((presence = GST_PAD_TEMPLATE_PRESENCE(templ)) != GST_PAD_ALWAYS) {
            if (GST_PAD_TEMPLATE_DIRECTION(templ) == GST_PAD_SRC) {
                addOutputPad(new OutputPad(
                    this, nullptr,
                    presence == GST_PAD_SOMETIMES ? Pad::Availability::SOMETIMES : Pad::Availability::ON_DEMAND));
            } else if (GST_PAD_TEMPLATE_DIRECTION(templ) == GST_PAD_SINK) {
                addInputPad(new InputPad(
                    this, nullptr,
                    presence == GST_PAD_SOMETIMES ? Pad::Availability::SOMETIMES : Pad::Availability::ON_DEMAND));
            }
        }

        templates = templates->next;
    }
}

void ElementNode::updateNode()
{
    for (auto in : m_input_pads) {
        if (in->isOutdated()) {
            auto peer = in->getPeerOfBase();
            if (peer)
                in->link(peer);
        }
    }
    for (auto out : m_output_pads) {
        if (out->isOutdated()) {
            auto peer = out->getPeerOfBase();
            if (peer)
                out->link(peer);
        }
    }
}

bool ElementNode::operator==(Element* elem)
{
    return elem == m_element;
}

std::vector<Element::Property*>& ElementNode::getProperties()
{
    return m_element->getProperties();
}

Gtk::Widget* ElementNode::createInfoWidget()
{
    auto info_panel = Gtk::make_managed<Gtk::ListBox>();
    info_panel->set_selection_mode(Gtk::SelectionMode::NONE);
    info_panel->append(*Gtk::make_managed<Gtk::Label>(Glib::ustring::sprintf("Properties of %s", getName().c_str())));

    auto properties = getProperties();
    for (auto prop : properties) {
        auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
        box->set_spacing(8);

        auto name = Gtk::make_managed<Gtk::Label>();
        name->set_markup(Glib::ustring::sprintf("<i>%s</i>", prop->getName()));

        auto default_value = prop->getValueAsString(Element::Property::ValueType::Default);
        auto default_type  = prop->getTypeAsString();
        auto value         = prop->getValueAsString();

        name->set_tooltip_text(prop->getDescription());

        auto entry = Gtk::make_managed<Gtk::Entry>();
        entry->set_hexpand(true);
        if (!value.empty())
            entry->set_text(value);
        entry->set_placeholder_text(default_value);
        entry->set_tooltip_text(Glib::ustring::sprintf("Format: %s\t\tDefault: %s", default_type, default_value));

        auto confirm_button = Gtk::make_managed<Gtk::Button>("Confirm");
        confirm_button->set_sensitive(false);

        entry->signal_changed().connect([confirm_button, entry, prop]() {
            auto value = prop->getValueAsString();
            if (value.empty())
                confirm_button->set_sensitive(entry->get_text_length() != 0);
            else
                confirm_button->set_sensitive(std::string(entry->get_text()) != value);
        });
        confirm_button->signal_clicked().connect([confirm_button, entry, prop] {
            prop->setValue(entry->get_text());
            confirm_button->set_sensitive(false);
        });

        box->append(*name);
        box->append(*entry);
        box->append(*confirm_button);

        info_panel->append(*box);
    }
    return info_panel;
}

Pad* ElementNode::searchForPeer(GstPad* pad)
{
    for (auto in : m_input_pads) {
        if (GST_PAD_PEER(pad) == in->getBase())
            return in;
    }
    for (auto out : m_output_pads) {
        if (GST_PAD_PEER(pad) == out->getBase())
            return out;
    }
    return nullptr;
}
