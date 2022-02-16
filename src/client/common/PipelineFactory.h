#pragma once

#include "client//Client.h"
#include "utils/FileUtils.h"

#include <glibmm/ustring.h>

class PipelineFactory{
public:
    static void setClient(Client* client) { s_client = client; }
    void createEmptyPipeline(Glib::ustring const& name);
    void createPipelineFromData(std::shared_ptr<FileUtils::file_info> data);

private:
    static Client* s_client;
};
