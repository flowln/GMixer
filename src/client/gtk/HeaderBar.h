#pragma once

#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/label.h>

class HeaderBar {
   public:
    static Gtk::HeaderBar* create();

    void setTitleText(const Glib::ustring text);

   protected:
    HeaderBar();

    void importButtonCallback();

   private:
    static HeaderBar* s_instance;

    Gtk::HeaderBar* m_bar;
    Gtk::Label* m_title;

    Gtk::Button* m_add_button;
    Gtk::Button* m_import_button;
    Gtk::Button* m_save_button;

    // Gtk::Button* m_control_pipeline;
};
