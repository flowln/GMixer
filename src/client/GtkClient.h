#pragma once

#include "client/Client.h"
#include "client/gtk/PipelineListModel.h"

class GtkClient final : public Client {
   public:
    GtkClient();
    virtual ~GtkClient() override;

    virtual bool runClient(std::string&& dbus_name, int argc, char* argv[]) override;

    virtual std::shared_ptr<Logger> loggingAgent() override;

    virtual PipelineListModel* pipelineStorageAgent() override;

   private:
    PipelineListModel* m_pipelines = nullptr;
};
