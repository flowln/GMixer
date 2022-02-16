#include <gst/gst.h>

#include "client/GtkClient.h"

int main(int argc, char* argv[])
{
    gst_init(&argc, &argv);

    Client* fend;
    if (true)  // FIXME: Insert check for GTK compatibility here
        fend = new GtkClient();

    return fend->runClient("org.flowln.GMixer", argc, argv);
}
