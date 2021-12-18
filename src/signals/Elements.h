#include <sigc++/signal.h>
#include <gtkmm/treemodel.h>

namespace Signals{
    sigc::signal<void(Glib::UStringView)>& element_add();
}
