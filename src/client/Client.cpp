#include "client/Client.h"

Client* Client::s_instance = nullptr;

Client::Client()
{
    s_instance = this;
}
