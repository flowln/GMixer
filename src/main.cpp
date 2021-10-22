#include <gtkmm/application.h>
#include <gst/gst.h>

#include "MainWindow.h"

int main (int argc, char *argv[])
{
    gst_init (&argc, &argv);

    auto app = Gtk::Application::create("org.flowln.GMixer");

    return app->make_window_and_run<MainWindow>(argc, argv);
}
