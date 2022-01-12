#include "gstreamer/ElementListModel.h"
#include "gstreamer/ElementUtils.h"

#include "signals/Pipelines.h"
#include "signals/Elements.h"

#include "client/gtk/ElementSelector.h"
#include "client/gtk/PipelineSelector.h"
#include "client/gtk/MainWindow.h"

#include <gtkmm/paned.h>

ElementSelector* ElementSelector::s_instance = nullptr;

ElementList::ElementList(const ElementType& type)
    : Gtk::ScrolledWindow()
{
    m_model = std::make_unique<ElementListModel>(type);
    m_view = std::make_unique<Gtk::TreeView>(m_model->getModel());
    m_view->set_enable_search(true);
    set_child(*m_view);
}

ElementRecord& ElementList::getModelRecord() const{ return m_model->getRecord(); };

OptionalInfo::OptionalInfo(const Glib::ustring& title)
    : Gtk::Expander()
{ 
    set_label(title);
    m_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    m_box->set_halign(Gtk::Align::BASELINE);
    set_child(*m_box); 
}

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
        [add_button](Pipeline* selected){
            add_button->set_sensitive(selected != nullptr);
        });
    add_button->signal_clicked().connect(
        [this]{
            Signals::element_add().emit(getSelectedElementName());
        });
    m_selected_info_box->append(*add_button);

    add(*m_selected_info_box, "selected");
    add(m_not_selected, "not_selected");

    set_visible_child("not_selected");
}
Glib::ustring SelectedInfoPanel::getSelectedPluginName() const
{
    return m_list->getSelection().get_selected()->get_value(m_list->getModelRecord().m_plugin);
}

Glib::ustring SelectedInfoPanel::getSelectedElementName() const
{
    return m_list->getSelection().get_selected()->get_value(m_list->getModelRecord().m_element_name);
}

void SelectedInfoPanel::selectionChanged()
{
    auto plugin_name = getSelectedPluginName();
    auto plugin_info = ElementUtils::getPluginInfo(plugin_name);

    m_plugin_name.set_text        (Glib::ustring::sprintf("\tName: %s", plugin_name));
    m_plugin_description.set_text (Glib::ustring::sprintf("\tDescription: %s", plugin_info->description));
    m_plugin_version.set_text     (Glib::ustring::sprintf("\tVersion: %s", plugin_info->version));
    m_plugin_license.set_text     (Glib::ustring::sprintf("\tLicense: %s", plugin_info->license));

    auto element_name = getSelectedElementName();
    auto element_info = ElementUtils::getElementInfo(element_name);

    m_name.set_text         (Glib::ustring::sprintf("\tName: %s", element_name));
    m_description.set_text  (Glib::ustring::sprintf("\tDescription: %s", element_info->description));
    m_author.set_text       (Glib::ustring::sprintf("\tAuthor: %s", element_info->author));

    m_src_num.set_text      (Glib::ustring::sprintf("\tNumber of source pads: %d", element_info->num_srcs));
    m_src_caps.set_text     ("");

    guint src_num = 1;
    for(auto src_pad : *element_info->src_pads){
        if(src_num > element_info->num_srcs) 
            break;

        m_src_caps.set_text(Glib::ustring::sprintf("%s\n\tSource pad #%d:\n\t\tCapabilities: %s\n\t\tAvailability: %s\n", 
                    m_src_caps.get_text(), src_num++, src_pad.caps, src_pad.availability));
    }

    m_sink_num.set_text     (Glib::ustring::sprintf("\tNumber of sink pads: %d", element_info->num_sinks));
    m_sink_caps.set_text    ("");

    guint sink_num = 1;
    for(auto sink_pad : *element_info->sink_pads){
        if(sink_num > element_info->num_sinks) 
            break;

        m_sink_caps.set_text(Glib::ustring::sprintf("%s\n\tSink pad #%d:\n\t\tCapabilities: %s\n\t\tAvailability: %s\n", 
                    m_sink_caps.get_text(), sink_num++, sink_pad.caps, sink_pad.availability));
    }

    set_visible_child("selected");
}

ElementSelector* ElementSelector::create(MainWindow* main_window)
{
    if(s_instance)
        return s_instance;

    s_instance = new ElementSelector(main_window);
    return s_instance;
}

ElementSelector::ElementSelector(Gtk::Window* main_window)
    : m_main_window(main_window)
{
    auto source_page = Gtk::make_managed<Gtk::Paned>();
    source_page->set_resize_end_child(false);

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
    filter_page->set_resize_end_child(false);

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
    sink_page->set_resize_end_child(false);

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

    m_notebook.set_hexpand(true);
    m_notebook.set_size_request(120, 280);

    m_notebook.insert_page(*source_page, *source_tab_name, 0);
    m_notebook.insert_page(*filter_page, *filter_tab_name, 1);
    m_notebook.insert_page(*sink_page,   *sink_tab_name,   2);

    // FIXME: The source page separator position is different from the filter / sink ones,
    // and I could not find a way to make them equal without throwing magical numbers everywhere...
    // Maybe if there was a way to make the source page behave the same way the other 2 pages? :(

    m_notebook.signal_switch_page().connect(
        [filter_page, filter_list, sink_page, sink_list](Gtk::Widget* page, guint p_num){
            if(page == filter_page){ 
                filter_list->getModel().populate(); 

                filter_page->set_position(filter_page->get_parent()->get_width()/2);
            }
            else if(page == sink_page){ 
                sink_list->getModel().populate();

                sink_page->set_position(sink_page->get_parent()->get_width()/2);
            }
        }); 

    source_list->getModel().populate();
    source_page->set_position(source_page->get_width()/2);
}

Glib::ustring ElementSelector::getSelectedElement()
{
    if(!s_instance)
        return {};

    auto& note = s_instance->m_notebook;
    auto curr_page = dynamic_cast<Gtk::Paned*>(note.get_nth_page(note.get_current_page()));
    if(!curr_page)
        return {};

    return dynamic_cast<SelectedInfoPanel*>(curr_page->get_end_child())->getSelectedElementName();
}
