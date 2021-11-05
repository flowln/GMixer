#include "backend/ElementListModel.h"
#include "backend/ElementUtils.h"
#include "ElementSelector.h"

//#include <iostream>

#include <gtkmm/paned.h>

ElementList::ElementList(const ElementType& type)
    : Gtk::ScrolledWindow()
{
    m_model = std::make_unique<ElementListModel>(type);
    m_view = std::make_unique<Gtk::TreeView>(m_model->getModel());
    m_view->set_enable_search(true);
    set_child(*m_view);
}

ElementRecord& ElementList::getModelRecord() const{ return m_model->getRecord(); };

SelectedInfoPanel::SelectedInfoPanel(ElementList* associated_list)
    : Gtk::Stack()
    , m_list(associated_list)
    , m_not_selected("No element is selected.")
{
    m_selected_info_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);

    m_name.set_xalign(0);
    m_element_info.append(m_name);

    m_description.set_xalign(0);
    m_element_info.append(m_description);

    m_origin.set_xalign(0);
    m_element_info.append(m_origin);

    m_selected_info_box->append(m_element_info);

    m_plugin_name.set_xalign(0);
    m_plugin_info.append(m_plugin_name);

    m_plugin_description.set_xalign(0);
    m_plugin_info.append(m_plugin_description);

    m_plugin_version.set_xalign(0);
    m_plugin_info.append(m_plugin_version);

    m_plugin_license.set_xalign(0);
    m_plugin_info.append(m_plugin_license);

    m_selected_info_box->append(m_plugin_info);

    add(*m_selected_info_box, "selected");
    add(m_not_selected, "not_selected");

    set_visible_child("not_selected");
}

void SelectedInfoPanel::selectionChanged()
{
    auto curr_selection = m_list->getSelection().get_selected();

    auto plugin_info = ElementUtils::getPluginInfo(curr_selection->get_value(m_list->getModelRecord().m_plugin).c_str());

    Glib::ustring plugin_name = "Name: ";
    plugin_name.append(plugin_info->name);
    Glib::ustring plugin_desc = "Description: ";
    plugin_desc.append(plugin_info->description);
    Glib::ustring plugin_ver = "Version: ";
    plugin_ver.append(plugin_info->version);
    Glib::ustring plugin_license = "License: ";
    plugin_license.append(plugin_info->license);

    m_plugin_name.set_text(plugin_name);
    m_plugin_description.set_text(plugin_desc);
    m_plugin_version.set_text(plugin_ver);
    m_plugin_license.set_text(plugin_license);

    //delete plugin_info;

    auto element_info = ElementUtils::getElementInfo(curr_selection->get_value(m_list->getModelRecord().m_element_name).c_str());

    Glib::ustring element_name = "Name: ";
    element_name.append(element_info->name);
    Glib::ustring element_desc = "Description: ";
    element_desc.append(element_info->description);

    m_name.set_text(element_name);
    m_description.set_text(element_desc);

    //delete element_info;

    set_visible_child("selected");
}

ElementSelector::ElementSelector(Gtk::Window* main_window)
    : m_main_window(main_window)
{
    auto source_page = Gtk::make_managed<Gtk::Paned>();

    auto source_list = Gtk::make_managed<ElementList>(ElementType::SOURCE);
    source_list->append_column("Element", source_list->getModelRecord().m_element_name);
    source_list->append_column("Plugin", source_list->getModelRecord().m_plugin);
    source_list->append_column("Package", source_list->getModelRecord().m_package);
    source_list->setAllSearchable();

    auto source_info = Gtk::make_managed<SelectedInfoPanel>(source_list);
    source_list->getSelection().signal_changed().connect(sigc::mem_fun(*source_info, &SelectedInfoPanel::selectionChanged));

    source_page->set_start_child(*source_list);
    source_page->set_end_child(*source_info);

    auto filter_page = Gtk::make_managed<Gtk::Paned>();

    auto filter_list = Gtk::make_managed<ElementList>(ElementType::FILTER);
    filter_list->append_column("Element", filter_list->getModelRecord().m_element_name);
    filter_list->append_column("Plugin", filter_list->getModelRecord().m_plugin);
    filter_list->append_column("Package", filter_list->getModelRecord().m_package);
    filter_list->setAllSearchable();

    auto filter_info = Gtk::make_managed<SelectedInfoPanel>(filter_list);
    filter_list->getSelection().signal_changed().connect(sigc::mem_fun(*filter_info, &SelectedInfoPanel::selectionChanged));

    filter_page->set_start_child(*filter_list);
    filter_page->set_end_child(*filter_info);

    auto sink_page = Gtk::make_managed<Gtk::Paned>();

    auto sink_list   = Gtk::make_managed<ElementList>(ElementType::SINK);
    sink_list->append_column("Element", sink_list->getModelRecord().m_element_name);
    sink_list->append_column("Plugin", sink_list->getModelRecord().m_plugin);
    sink_list->append_column("Package", sink_list->getModelRecord().m_package);
    sink_list->setAllSearchable();

    auto sink_info = Gtk::make_managed<SelectedInfoPanel>(sink_list);
    sink_list->getSelection().signal_changed().connect(sigc::mem_fun(*sink_info, &SelectedInfoPanel::selectionChanged));

    sink_page->set_start_child(*sink_list);
    sink_page->set_end_child(*sink_info);

    auto source_tab_name = Gtk::make_managed<Gtk::Label>("Sources");
    auto filter_tab_name = Gtk::make_managed<Gtk::Label>("Filters");
    auto sink_tab_name   = Gtk::make_managed<Gtk::Label>("Sinks");

    m_notebook = Gtk::make_managed<Gtk::Notebook>();
    m_notebook->insert_page(*source_page, *source_tab_name, 0);
    m_notebook->insert_page(*filter_page, *filter_tab_name, 1);
    m_notebook->insert_page(*sink_page,   *sink_tab_name,   2);

    source_list->getModel().populate();
    filter_list->getModel().populate();
    sink_list->getModel().populate();
}
