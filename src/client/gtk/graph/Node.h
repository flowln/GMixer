#pragma once

#include "client/gtk/graph/Property.h"

#include <gtkmm/widget.h>
#include <glibmm/ustring.h>
#include <cairomm/cairomm.h>
#include <forward_list>

//Forward-declarations
class GraphViewer;
class Pad;
class InputPad;
class OutputPad;

/** This class represents a node in a graph.
 *  Such entity has a descriptive name and a coordinate in the graph
 *  it belongs to, as well as a width and a height. By default it is
 *  drawn as a rectangle, with the top-left corner as (0,0).
 *  It relates to other nodes via input and/or output pads.
 *  */
class Node {
    public:
        Node(GraphViewer* parent, Glib::ustring name, double x, double y);
        virtual ~Node() = default;

        /** Destroys the node and frees it. Use this method instead of `delete node;`.*/
        void destroy();

        /** Set new position of the node, update its pads and update the node. */
        void setPosition(double new_x, double new_y);

        double getX() const { return m_x; }
        double getY() const { return m_y; }

        /** Set new size of the node, update its pads and update the node. */
        void setSize(double new_width, double new_height);

        double getWidth() const { return m_width; }
        double getHeight() const { return m_height; }

        /** Add an input pad to the node's inputs.
         *  Also automatically updates all the current 
         *  inputs geometries to accomodate the new one. */
        void addInputPad(InputPad* pad);

        /** Add an output pad to the node's outputs.
         *  Also automatically updates all the current 
         *  outputs geometries to accomodate the new one. */
        void addOutputPad(OutputPad* pad);

        /** Get the vector of the node's input pads. */
        std::vector<InputPad*>& getInputPads() { return m_input_pads; };

        /** Get the vector of the node's output pads. */
        std::vector<OutputPad*>& getOutputPads() { return m_output_pads; };

        /** Get the currently selected pad in the node, or nullptr if none is selected. */
        Pad* selectedPad() const { return m_selected_pad; }

        /** Set a callback for when the node is updated. 
         *  The node is updated when:
         *      - It is selected / deselected.
         *      - It's geometry changes (position or size).
         *  */
        void onUpdateCallback(sigc::slot<void(void)> cb) { update_callback = cb; };

        /** Respond to a click event inside of the node at coordinate (x,y).
         *  Possible outcomes are:
         *      - A pad was clicked. In this case it will start / finish linking and return false.
         *      - No pad was clicked. In this case the node will become selected and return true 
         *      only if it was not previously selected.
         *
         *  This function assumes the (x,y) is already inside the node's boundaries.
         *  If (x,y) is outside on the node, the node will simply be selected as if was clicked outside of a pad.*/
        bool onClick(double x, double y);

        /** Checks if (x,y) is inside the node's boundaries. */
        bool contains(double x, double y) const;

        /** Draws the node and tell all its pads to draw themselves (thorough their draw() interface). */
        void draw(const Cairo::RefPtr<Cairo::Context>& cr) const;

        /** Deselects a node. If it was not selected, this does nothing. */
        void deselect();

        /** Stop linking if it has a linking pad. Does nothing otherwise. */
        void stopLinking();

        bool isSelected() const { return m_is_selected; }

        Glib::ustring getName() const { return m_name.getName(); }
        GraphViewer* getGraph() const { return m_parent; }

        /** Get all the node's properties.
         *  While the base Node only has a 'name' property, children of 
         *  the Node class can have as many properties as they want.
         *  */
        virtual GMixer::PropertyList* getProperties();
        virtual bool updateProperty(GMixer::Property*, const std::string&, const std::string&){ return false; };

    protected:
        /* To try to select a node, use onClick.*/
        void select();

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
