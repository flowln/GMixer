#include <sigc++/signal.h>
#include <gtkmm/treemodel.h>

class Pipeline;
namespace Signals{
    sigc::signal<void(Pipeline*)>&              pipeline_selected();
//    sigc::signal<void(Gtk::TreeModel::Path)>&   pipeline_added();
}
