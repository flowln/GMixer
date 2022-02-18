#include "client/GtkClient.h"

#include <gtkmm/application.h>
#include "client/cmd/CommandLineLogger.h"
#include "client/gtk/MainWindow.h"
#include "client/gtk/PipelineListModel.h"
#include "client/gtk/PipelineSelector.h"

GtkClient::GtkClient() : Client()
{
    // TODO: Add GTK Logger
    m_logger    = std::make_shared<CommandLineLogger>(Logger::LoggingLevel::WARNING);
    m_pipelines = PipelineListModel::create();
    PipelineSelector::setClient(this);
}

GtkClient::~GtkClient()
{
    if (m_pipelines)
        delete m_pipelines;
}

bool GtkClient::runClient(std::string&& dbus_name, int argc, char* argv[])
{
    auto app = Gtk::Application::create(dbus_name);
    app->register_application();

    return app->make_window_and_run<MainWindow>(argc, argv);
}

std::shared_ptr<Logger> GtkClient::loggingAgent()
{
    return m_logger;
}

PipelineListModel* GtkClient::pipelineStorageAgent()
{
    return m_pipelines;
}
