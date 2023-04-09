#include "gstreamer/ElementUtils.h"

#include "signals/Elements.h"
#include "signals/Pipelines.h"

#include "client/gtk/ElementSelector.h"
#include "client/gtk/MainWindow.h"

#include "client/gtk/widgets/GstElementList.h"

#include <gtkmm/button.h>
#include <gtkmm/paned.h>

ElementSelector* ElementSelector::create(MainWindow* parent)
{
    return Gtk::make_managed<ElementSelector>(parent);
}

ElementSelector::ElementSelector(Gtk::Window* parent) : Gtk::Notebook(), m_parent(parent)
{
    auto source_page = Gtk::make_managed<Gtk::Paned>();
    source_page->set_shrink_start_child(false);
    source_page->set_shrink_end_child(false);

    auto source_list = Gtk::make_managed<ElementList>(ElementType::SOURCE);

    auto source_info = Gtk::make_managed<SelectedInfoPanel>(source_list);
    source_list->when_selection_changed(sigc::mem_fun(*source_info, &SelectedInfoPanel::selectionChanged));

    source_page->set_start_child(*source_list);
    source_page->set_end_child(*source_info);

    auto filter_page = Gtk::make_managed<Gtk::Paned>();
    filter_page->set_shrink_start_child(false);
    filter_page->set_shrink_end_child(false);

    auto filter_list = Gtk::make_managed<ElementList>(ElementType::FILTER);

    auto filter_info = Gtk::make_managed<SelectedInfoPanel>(filter_list);
    filter_list->when_selection_changed(sigc::mem_fun(*filter_info, &SelectedInfoPanel::selectionChanged));

    filter_page->set_start_child(*filter_list);
    filter_page->set_end_child(*filter_info);

    auto sink_page = Gtk::make_managed<Gtk::Paned>();
    sink_page->set_shrink_start_child(false);
    sink_page->set_shrink_end_child(false);

    auto sink_list = Gtk::make_managed<ElementList>(ElementType::SINK);

    auto sink_info = Gtk::make_managed<SelectedInfoPanel>(sink_list);
    sink_list->when_selection_changed(sigc::mem_fun(*sink_info, &SelectedInfoPanel::selectionChanged));

    sink_page->set_start_child(*sink_list);
    sink_page->set_end_child(*sink_info);

    auto source_tab_name = Gtk::make_managed<Gtk::Label>("Sources");
    auto filter_tab_name = Gtk::make_managed<Gtk::Label>("Filters");
    auto sink_tab_name   = Gtk::make_managed<Gtk::Label>("Sinks");

    set_hexpand(true);

    insert_page(*source_page, *source_tab_name, 0);
    insert_page(*filter_page, *filter_tab_name, 1);
    insert_page(*sink_page, *sink_tab_name, 2);

    signal_switch_page().connect([filter_list, sink_list](Gtk::Widget*, guint p_num) {
        switch (p_num) {
            case 1:
                filter_list->populate();
                break;
            case 2:
                sink_list->populate();
                break;
        }
    });

    source_list->populate();
}

SelectedInfoPanel::OptionalInfo::OptionalInfo(std::string&& title) : Gtk::Expander()
{
    set_label(title);
    m_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    m_box->set_halign(Gtk::Align::BASELINE);
    set_child(*m_box);
}

SelectedInfoPanel::SelectedInfoPanel(ElementList* associated_list)
    : Gtk::Stack(), m_element_list(associated_list), m_not_selected("No element is selected.")
{
    set_size_request(300, -1);

    m_selected_info_box = Gtk::make_managed<Gtk::ListBox>();
    m_selected_info_box->set_selection_mode(Gtk::SelectionMode::NONE);

    m_element_info.appendMany(&m_name, &m_description, &m_author);
    m_selected_info_box->append(m_element_info);

    m_plugin_info.appendMany(&m_plugin_name, &m_plugin_description, &m_plugin_version, &m_plugin_license);
    m_selected_info_box->append(m_plugin_info);

    m_pads_info.appendMany(&m_src_num, &m_src_caps, &m_sink_num, &m_sink_caps);
    m_selected_info_box->append(m_pads_info);

    auto add_button = Gtk::make_managed<Gtk::Button>("Add");
    add_button->set_sensitive(false);
    Signals::pipeline_selected().connect(
        [add_button](Pipeline* selected) { add_button->set_sensitive(selected != nullptr); });
    add_button->signal_clicked().connect(
        [this] { Signals::element_add().emit(m_element_list->getCurrentElementName()); });
    m_selected_info_box->append(*add_button);

    add(*m_selected_info_box, "selected");
    add(m_not_selected, "not_selected");

    set_visible_child("not_selected");
}

void SelectedInfoPanel::selectionChanged(guint /* current selection index */)
{
    auto plugin_name = m_element_list->getCurrentElementPluginName();
    auto plugin_info = ElementUtils::getPluginInfo(plugin_name);

    m_plugin_name.set_text(Glib::ustring::sprintf("\tName: %s", plugin_name));
    m_plugin_description.set_text(Glib::ustring::sprintf("\tDescription: %s", plugin_info->description));
    m_plugin_version.set_text(Glib::ustring::sprintf("\tVersion: %s", plugin_info->version));
    m_plugin_license.set_text(Glib::ustring::sprintf("\tLicense: %s", plugin_info->license));

    auto element_name = m_element_list->getCurrentElementName();
    auto element_info = ElementUtils::getElementInfo(element_name);

    m_name.set_text(Glib::ustring::sprintf("\tName: %s", element_name));
    m_description.set_text(Glib::ustring::sprintf("\tDescription: %s", element_info->description));
    m_author.set_text(Glib::ustring::sprintf("\tAuthor: %s", element_info->author));

    m_src_num.set_text(Glib::ustring::sprintf("\tNumber of source pads: %d", element_info->num_srcs));
    m_src_caps.set_text("");

    guint src_num = 1;
    for (auto src_pad : *element_info->src_pads) {
        if (src_num > element_info->num_srcs)
            break;

        m_src_caps.set_text(
            Glib::ustring::sprintf("%s\n\tSource pad #%d:\n\t\tCapabilities: %s\n\t\tAvailability: %s\n",
                                   m_src_caps.get_text(), src_num++, src_pad.caps, src_pad.availability));
    }

    m_sink_num.set_text(Glib::ustring::sprintf("\tNumber of sink pads: %d", element_info->num_sinks));
    m_sink_caps.set_text("");

    guint sink_num = 1;
    for (auto sink_pad : *element_info->sink_pads) {
        if (sink_num > element_info->num_sinks)
            break;

        m_sink_caps.set_text(Glib::ustring::sprintf("%s\n\tSink pad #%d:\n\t\tCapabilities: %s\n\t\tAvailability: %s\n",
                                                    m_sink_caps.get_text(), sink_num++, sink_pad.caps,
                                                    sink_pad.availability));
    }

    set_visible_child("selected");
}
