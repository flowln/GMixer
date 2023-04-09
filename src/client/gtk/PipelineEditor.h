#pragma once

#include "gstreamer/Element.h"

#include <gtkmm/actionbar.h>
#include <gtkmm/paned.h>
#include <gtkmm/box.h>
#include <gtkmm/listbox.h>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/togglebutton.h>

// Forward-declarations
class Pipeline;
class Node;
class GraphViewer;
class PipelineEditor;

enum class OperationMode {
    MODE_SELECT,
    MODE_MOVE,
    MODE_CUT,
    MODE_DELETE
};

class PipelineEditor final : public Gtk::Paned {
public:
    PipelineEditor(Gtk::Widget& parent, Pipeline* pipeline);

    /* Return the pipeline associated with this editor. */
    Pipeline* getPipeline() const { return m_pipeline; }
    /* Return currently selected editing mode. */
    OperationMode getMode() const { return m_graph.getMode(); };

private:

    /* This widget contains:
     * - Representetion of the selected pipeline in graph form
     * - ActionBar with editing and playback options for the pipeline / its graph
     */
    class PipelineGraph final : public Gtk::Box {
    public:
        PipelineGraph(PipelineEditor* parent);

        /* FIXME: Add better way to add elements to the graph */
        void addElement(Element*);

        OperationMode getMode() const { return m_mode; };
        void setMode(OperationMode new_mode);

    private:
        OperationMode m_mode;
        PipelineEditor* m_editor;

        GraphViewer* m_viewer;
        Gtk::ActionBar m_bar;

        Gtk::ToggleButton m_mode_select;
        Gtk::ToggleButton m_mode_move;
        Gtk::ToggleButton m_mode_cut;
        Gtk::ToggleButton m_mode_delete;
    };

    /* This widget contains:
     * - List of editable properties of a selected element
     */
    class ElementPropertyEditor final : public Gtk::ScrolledWindow {
    public:
        ElementPropertyEditor(Gtk::Widget& parent);
        /* Show properties of the argument. */
        void hook(Node*);
    };

    Pipeline* m_pipeline;
    PipelineGraph m_graph;
    ElementPropertyEditor m_info;

    bool m_is_selected = true;
};
