#pragma once

#include "views/graph/Property.h"

#include <glibmm/ustring.h>
#include <cairomm/cairomm.h>
#include <forward_list>

//Forward-declarations
class GraphViewer;
class Pad;
class InputPad;
class OutputPad;

class Node {
    public:
        Node(GraphViewer* parent, Glib::ustring name, int x, int y);
        virtual ~Node() = default;

        void setPosition(int new_x, int new_y);

        void addInputPad(InputPad* pad);
        void addOutputPad(OutputPad* pad);
        std::vector<InputPad*>& getInputPads() { return m_input_pads; };
        std::vector<OutputPad*>& getOutputPads() { return m_output_pads; };
        Pad* selectedPad() const { return m_selected_pad; }

        void onUpdateCallback(sigc::slot<void(void)> cb) { update_callback = cb; };
        void onClick(double x, double y);

        void draw(const Cairo::RefPtr<Cairo::Context>& cr) const;
        bool isSelected() const { return m_is_selected; }
        void select();
        void deselect();

        Glib::UStringView getName() const { return m_name.name; }
        int getX() const { return m_x; }
        int getY() const { return m_y; }
        GraphViewer* getGraph() const { return m_parent; }
        virtual const std::unique_ptr<GMixer::PropertyList> getProperties();

        bool contains(double x, double y) const;

    protected:
        void setSize(double new_width, double new_height);
        void updateInputPadGeometry(InputPad*, int start_index = -1);
        void updateOutputPadGeometry(OutputPad*, int start_index = -1);

        inline void drawInputs(const Cairo::RefPtr<Cairo::Context>& cr) const;
        inline void drawOutputs(const Cairo::RefPtr<Cairo::Context>& cr) const;
        inline void drawName(const Cairo::RefPtr<Cairo::Context>& cr) const;

        int numOfInputs() const { return m_input_pads.size(); }
        int numOfOutputs() const { return m_output_pads.size(); }

        GraphViewer* m_parent;

        GMixer::Property m_name;
        double m_x, m_y;
        double m_width, m_height;

        bool m_is_selected = false;

        std::vector<InputPad*> m_input_pads;
        std::vector<OutputPad*> m_output_pads;
        Pad* m_selected_pad = nullptr;

        sigc::slot<void(void)> update_callback;
};
