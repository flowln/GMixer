#include "backend/ElementListModel.h"
#include "backend/ElementUtils.h"
#include "ElementSelector.h"

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

template <typename... T>
void OptionalInfo::appendMany(Gtk::Label* widg, T... widgs)
{
    widg->set_xalign(0);

    append(widg);
    appendMany(widgs...);
}

SelectedInfoPanel::SelectedInfoPanel(ElementList* associated_list)
    : Gtk::Stack()
    , m_list(associated_list)
    , m_not_selected("No element is selected.")
{
    m_selected_info_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);

    m_element_info.appendMany(&m_name, &m_description, &m_author);
    m_selected_info_box->append(m_element_info);

    m_plugin_info.appendMany(&m_plugin_name, &m_plugin_description, &m_plugin_version, &m_plugin_license);
    m_selected_info_box->append(m_plugin_info);

    m_pads_info.appendMany(&m_src_num, &m_src_caps, &m_sink_num, &m_sink_caps);
    m_selected_info_box->append(m_pads_info);

    add(*m_selected_info_box, "selected");
    add(m_not_selected, "not_selected");

    set_visible_child("not_selected");
}

void SelectedInfoPanel::selectionChanged()
{
    auto curr_selection = m_list->getSelection().get_selected();

    auto plugin_name = curr_selection->get_value(m_list->getModelRecord().m_plugin);
    auto plugin_info = ElementUtils::getPluginInfo(plugin_name);

    m_plugin_name.set_text        (Glib::ustring::sprintf("Name: %s", plugin_name));
    m_plugin_description.set_text (Glib::ustring::sprintf("Description: %s", plugin_info->description));
    m_plugin_version.set_text     (Glib::ustring::sprintf("Version: %s", plugin_info->version));
    m_plugin_license.set_text     (Glib::ustring::sprintf("License: %s", plugin_info->license));

    auto element_name = curr_selection->get_value(m_list->getModelRecord().m_element_name);
    auto element_info = ElementUtils::getElementInfo(element_name);

    m_name.set_text         (Glib::ustring::sprintf("Name: %s", element_name));
    m_description.set_text  (Glib::ustring::sprintf("Description: %s", element_info->description));
    m_author.set_text       (Glib::ustring::sprintf("Author: %s", element_info->author));

    m_src_num.set_text      (Glib::ustring::sprintf("Number of source pads: %d", element_info->num_srcs));
    m_src_caps.set_text     ("");

    guint src_num = 1;
    for(auto src_pad : *element_info->src_pads){
        if(src_num > element_info->num_srcs) 
            break;

        m_src_caps.set_text(Glib::ustring::sprintf("%s\nSource pad #%d:\n\tCapabilities: %s\n\tAvailability: %s\n", 
                    m_src_caps.get_text(), src_num++, src_pad.caps, src_pad.availability));
    }

    m_sink_num.set_text     (Glib::ustring::sprintf("Number of sink pads: %d", element_info->num_sinks));
    m_sink_caps.set_text    ("");

    guint sink_num = 1;
    for(auto sink_pad : *element_info->sink_pads){
        if(sink_num > element_info->num_sinks) 
            break;

        m_sink_caps.set_text(Glib::ustring::sprintf("%s\nSink pad #%d:\n\tCapabilities: %s\n\tAvailability: %s\n", 
                    m_sink_caps.get_text(), sink_num++, sink_pad.caps, sink_pad.availability));
    }

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

    m_notebook->signal_switch_page().connect(
        [filter_page, filter_list, sink_page, sink_list](Gtk::Widget* page, guint p_num){
            if(page == filter_page) filter_list->getModel().populate();
            else if(page == sink_page) sink_list->getModel().populate();
        }); 

    source_list->getModel().populate();
}
