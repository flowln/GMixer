#include "ElementSelector.h"
#include "gtkmm/object.h"

ElementList::ElementList(const ElementType& type)
    : Gtk::ScrolledWindow()
{
    m_model = std::make_unique<ElementListModel>(type);
    m_view = std::make_unique<Gtk::TreeView>(m_model->getModel());
    set_child(*m_view);
}

ElementSelector::ElementSelector(Gtk::Window* main_window)
    : m_main_window(main_window)
{
    auto source_page = Gtk::make_managed<ElementList>(ElementType::SOURCE);
    source_page->append_column("Element", source_page->getModelRecord().m_element_name);
    source_page->append_column("Plugin", source_page->getModelRecord().m_plugin);
    source_page->append_column("Package", source_page->getModelRecord().m_package);

    auto filter_page = Gtk::make_managed<ElementList>(ElementType::FILTER);
    filter_page->append_column("Element", filter_page->getModelRecord().m_element_name);
    filter_page->append_column("Plugin", filter_page->getModelRecord().m_plugin);
    filter_page->append_column("Package", filter_page->getModelRecord().m_package);

    auto sink_page   = Gtk::make_managed<ElementList>(ElementType::SINK);
    sink_page->append_column("Element", sink_page->getModelRecord().m_element_name);
    sink_page->append_column("Plugin", sink_page->getModelRecord().m_plugin);
    sink_page->append_column("Package", sink_page->getModelRecord().m_package);

    auto source_tab_name = Gtk::make_managed<Gtk::Label>("Sources");
    auto filter_tab_name = Gtk::make_managed<Gtk::Label>("Filters");
    auto sink_tab_name   = Gtk::make_managed<Gtk::Label>("Sinks");

    m_notebook = Gtk::make_managed<Gtk::Notebook>();
    m_notebook->insert_page(*source_page, *source_tab_name, 0);
    m_notebook->insert_page(*filter_page, *filter_tab_name, 1);
    m_notebook->insert_page(*sink_page,   *sink_tab_name,   2);

    source_page->getModel().populate();
    filter_page->getModel().populate();
    sink_page->getModel().populate();
}
