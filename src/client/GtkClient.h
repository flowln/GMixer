#pragma once

#include <string>

namespace GtkClient{
    bool runClient(std::string&& dbus_name, int argc, char* argv[]);
}
