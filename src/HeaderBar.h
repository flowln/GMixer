#pragma once

#include "sigc++/functors/slot_base.h"
#include <glibmm/refptr.h>

#include <sigc++/functors/ptr_fun.h>

#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>

class HeaderBar {
    public:
        static Gtk::HeaderBar* create();

        static void setTitleText(const Glib::ustring text);
        static void setAddButtonCallback(sigc::slot<void(void)> const& callback);

    protected:
        HeaderBar();

    private:
        static HeaderBar* s_instance;

        Gtk::HeaderBar* m_bar;
        Gtk::Label* m_title;

        Gtk::Button* m_add_button;
};
