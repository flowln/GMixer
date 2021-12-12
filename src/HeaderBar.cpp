#include "HeaderBar.h"

HeaderBar* HeaderBar::s_instance = nullptr;

Gtk::HeaderBar* HeaderBar::create()
{
    if(s_instance)
        return s_instance->m_bar;

    s_instance = new HeaderBar();
    return s_instance->m_bar;
}

void HeaderBar::setTitleText(const Glib::ustring text)
{
    if(!s_instance)
        return;
    s_instance->m_title->set_text(text);
}


void HeaderBar::setAddButtonCallback(sigc::slot<void(void)> const& callback)
{
    if(!s_instance)
        return;

    s_instance->m_add_button->signal_clicked().connect(callback);
}

HeaderBar::HeaderBar()
{
    m_title = Gtk::make_managed<Gtk::Label>();
    m_add_button = Gtk::make_managed<Gtk::Button>("+");

    m_bar = Gtk::make_managed<Gtk::HeaderBar>();
    m_bar->set_title_widget(*m_title);
    m_bar->pack_start(*m_add_button);
}
