#pragma once

#include "client/Client.h"

class GtkClient final : public Client {
public:
    GtkClient();
    virtual bool runClient(std::string&& dbus_name, int argc, char* argv[]) override;
};
