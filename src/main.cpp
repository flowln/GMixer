#include <gst/gst.h>

#include "client/GtkClient.h"

int main (int argc, char *argv[])
{
    gst_init (&argc, &argv);
    return GtkClient::runClient("org.flowln.GMixer", argc, argv);
}
