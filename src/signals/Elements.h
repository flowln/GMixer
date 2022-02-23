#include <sigc++/signal.h>
#include <string>

namespace Signals {
sigc::signal<void(std::string)>& element_add();
}
