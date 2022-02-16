#include "client/gtk/Dialogs.h"

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/filechoosernative.h>
#include <gtkmm/label.h>

Gtk::Window* Dialog::s_main_window = nullptr;
PipelineFactory* Dialog::s_factory = nullptr;

void Dialog::createPipeline()
{
    if(!s_main_window || !s_factory)
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
    dialog->add_action_widget(*create_button, GTK_RESPONSE_ACCEPT);

    name_entry->signal_changed().connect(
        [create_button, name_entry](){
            if(name_entry->get_text_length() == 0)
                create_button->set_sensitive(false);
            else
                create_button->set_sensitive(true);
        });

    dialog->signal_response().connect(
        [dialog, name_entry](int response_id){
            if(response_id == GTK_RESPONSE_ACCEPT){
               s_factory->createEmptyPipeline(name_entry->get_text());
            }

            dialog->close();
        });
    
    dialog->show();
}
void Dialog::importPipelineFromFile()
{
    if(!s_main_window)
        return;
   
    auto dialog = Gtk::FileChooserNative::create("Please select the pipeline to import (.pipeline).", *s_main_window, Gtk::FileChooser::Action::OPEN, "Select", "Cancel");
    dialog->set_modal(true);
    dialog->set_transient_for(*s_main_window);

    auto filter = Gtk::FileFilter::create();
    filter->add_mime_type("text/plain");
    dialog->set_filter(filter);

    dialog->signal_response().connect(
        [dialog](int id){
            if(id == GTK_RESPONSE_ACCEPT){
                auto file = dialog->get_file();
                auto data = FileUtils::parseFile(file);

                if(!data)
                    return;
    
                s_factory->createPipelineFromData(data);
            }
        });

    dialog->show();
}
