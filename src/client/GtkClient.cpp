#include "client/GtkClient.h"

#include <gtkmm/application.h>
#include "client/cmd/CommandLineLogger.h"
#include "client/gtk/MainWindow.h"

GtkClient::GtkClient() {}

bool GtkClient::runClient(std::string&& dbus_name, int argc, char* argv[])
{
    auto app = Gtk::Application::create(dbus_name);
    app->register_application();

    return app->make_window_and_run<MainWindow>(argc, argv);
}
std::shared_ptr<Logger> GtkClient::loggingAgent()
{
    // TODO: Add GTK Logger
    return std::make_shared<CommandLineLogger>(Logger::LoggingLevel::WARNING);
}
