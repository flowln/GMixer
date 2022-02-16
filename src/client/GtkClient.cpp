#include "client/GtkClient.h"

#include "client/gtk/MainWindow.h"
#include <gtkmm/application.h>

GtkClient::GtkClient()
{

}

bool GtkClient::runClient(std::string&& dbus_name, int argc, char* argv[])
{
    auto app = Gtk::Application::create(dbus_name);
    app->register_application();

    return app->make_window_and_run<MainWindow>(argc, argv);
}
