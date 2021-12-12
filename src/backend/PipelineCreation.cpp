#include "backend/PipelineCreation.h"

#include <gtkmm/application.h>
#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/boxlayout.h>

Gtk::Window* PipelineFactory::s_main_window = nullptr;

void PipelineFactory::createPipeline(sigc::slot<void(Glib::RefPtr<Pipeline>)> const& callback)
{
    if(!s_main_window)
        return;

    auto dialog = new Gtk::Dialog("Create a new pipeline", true, true);
    dialog->set_transient_for(*s_main_window);
    dialog->set_default_size(s_main_window->get_width() / 5, s_main_window->get_height() / 8);
    dialog->get_content_area()->set_spacing(8);
    dialog->get_content_area()->set_margin(6);

    auto name_label = Gtk::make_managed<Gtk::Label>("Name of pipeline: ");
    dialog->get_content_area()->append(*name_label);

    auto name_entry = Gtk::make_managed<Gtk::Entry>();
    name_entry->set_hexpand(true);
    name_entry->set_placeholder_text("...");
    dialog->get_content_area()->append(*name_entry);

    auto close_button = Gtk::make_managed<Gtk::Button>("Cancel");
    dialog->add_action_widget(*close_button, GTK_RESPONSE_CANCEL);

    auto create_button = Gtk::make_managed<Gtk::Button>("Create");
    create_button->set_sensitive(false);
    dialog->add_action_widget(*create_button, GTK_RESPONSE_OK);

    name_entry->signal_changed().connect(
        [create_button, name_entry](){
            if(name_entry->get_text_length() == 0)
                create_button->set_sensitive(false);
            else
                create_button->set_sensitive(true);
        });

    dialog->signal_response().connect(
        [callback, dialog, name_entry](int response_id){
            if(response_id == GTK_RESPONSE_OK){
                auto name = name_entry->get_text();
                callback(Glib::make_refptr_for_instance(new Pipeline(name)));
            }

            dialog->close();
        });
    
    dialog->show();

}
