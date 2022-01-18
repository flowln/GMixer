#include <sigc++/signal.h>

class Node;

namespace Signals{
    sigc::signal<void(Node*)> node_selected();
}
