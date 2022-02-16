#include "client/Client.h"

#include "client/common/PipelineFactory.h"

Client::Client()
{
    PipelineFactory::setClient(this);
}
