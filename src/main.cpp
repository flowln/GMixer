#include <gtkmm/application.h>
#include <gst/gst.h>

#include "client/MainWindow.h"

int main (int argc, char *argv[])
{
    gst_init (&argc, &argv);

    auto app = Gtk::Application::create("org.flowln.GMixer");
    app->register_application();

    return app->make_window_and_run<MainWindow>(argc, argv);
}
