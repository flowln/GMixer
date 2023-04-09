#pragma once

#include <gtkmm/box.h>
#include <gtkmm/columnview.h>
#include <gtkmm/scrolledwindow.h>

class ElementListModel;

enum class ElementType { SOURCE, FILTER, SINK };

/* A scrollable list of Gst elements */
class ElementList : public Gtk::Box {
   public:
    ElementList(ElementType type);

    std::string getCurrentElementName() const;
    std::string getCurrentElementPluginName() const;
    std::string getCurrentElementPackage() const;

    void populate();

    /* Connect a callback to be called whenever the selection changes.
     * The argument in the callback is the new position (index) of the selection.
     */
    void when_selection_changed(std::function<void(guint)>);

   private:
    Gtk::ScrolledWindow m_window;
    Gtk::ColumnView* m_view = nullptr;

    std::unique_ptr<ElementListModel> m_model;
};

