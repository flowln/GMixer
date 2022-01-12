#pragma once

#include "client/graph/Property.h"

#include <gtkmm/actionbar.h>
#include <gtkmm/paned.h>
#include <gtkmm/box.h>
#include <gtkmm/listbox.h>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>

// Forward-declarations
class Pipeline;
class Element;
class Node;
class GraphViewer;
class PipelineEditor;

enum class OperationMode{
    MODE_SELECT,
    MODE_MOVE,
    MODE_CUT
};

class PipelineGraph : public Gtk::Box {
    public:
        PipelineGraph(PipelineEditor* parent);

        void addElement(Element*);
        OperationMode getMode() const { return m_mode; };

    private:
        OperationMode m_mode;
        PipelineEditor* m_editor;

        GraphViewer* m_viewer;
        Gtk::ActionBar* m_bar;
};

class ElementPropertyEditor : public Gtk::ScrolledWindow {
    public:
        ElementPropertyEditor(Gtk::Widget& parent);
        void hook(Node*);

    private:
        static Gtk::Box* createWidget(GMixer::Property*);

        Gtk::ListBox* m_properties = nullptr;
        Node* m_hooked_node = nullptr;
        GMixer::PropertyList* m_hooked_properties = nullptr;
};

class PipelineEditor : public Gtk::Paned {
    public:
        PipelineEditor(Gtk::Widget& parent, Pipeline* pipeline);

        Pipeline* getPipeline() const { return m_pipeline; }

    private:
        Pipeline* m_pipeline;
        PipelineGraph* m_graph;
        ElementPropertyEditor* m_properties;

        bool m_is_selected = true;
};
