#include <gst/gst.h>

#include "ProjectConfiguration.h"

#ifdef GTK_ENABLED
#include "client/GtkClient.h"
#else
#include "client/Client.h"
#endif

int main(int argc, char* argv[])
{
    gst_init(&argc, &argv);

    Client* fend = nullptr;

#ifdef GTK_ENABLED
    fend = new GtkClient();
#endif

    if (!fend) {
        printf("No client available! Please check your build configuration with `meson configure`.\n");
        return 1;
    }

    return fend->runClient("org.flowln.GMixer", argc, argv);
}
