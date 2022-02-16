#pragma once

#include <string>
#include <unordered_map>

class Client{
public:
    Client();
    virtual ~Client() = default;
    virtual bool runClient(std::string&& dbus_name, int argc, char* argv[]) = 0;
    std::string_view getSettings(std::string&& key);

protected:
    std::unordered_map<std::string, std::string> settings;
};
