#pragma once

#include <glibmm/ustring.h>
#include <cairomm/cairomm.h>

//Forward-declaration
class GraphViewer;

class Node {
    public:
        Node(GraphViewer* parent, Glib::ustring name, int x, int y);
        virtual ~Node() = default;

        void setPosition();
        void changePosition(int offset_x, int offset_y);

        void onUpdateCallback(sigc::slot<void(void)> cb) { update_callback = cb; };
        void onLinkCallback(sigc::slot<void(bool, Node*, int)> cb) { link_callback = cb; };
        void onClick(double x, double y);

        void draw(const Cairo::RefPtr<Cairo::Context>& cr) const;
        virtual void select();

        int getX() const { return m_x; }
        int getY() const { return m_y; }

        bool contains(double x, double y) const;

        /** 
         * Index of IO pad at (x, y).
         *
         * This gets the index, from top to bottom, of the pad at (x, y). 
         * Used to determine which pad was clicked, if any at all.
         *
         * @return The pad index at (x, y), -1 if there's no pad in such point.
         * Note that, for inputs, the actual index is the number returned, 
         * but for outputs, it is given by returned_index - number_of_inputs.
         * */
        int numberOfIOPad(double x, double y) const;
        std::pair<double, double> IOPadToPosition(bool is_input, int index_pad) const;

    protected:
        void setSize(int new_width, int new_height);

        inline void drawInputs(const Cairo::RefPtr<Cairo::Context>& cr) const;
        inline void drawOutputs(const Cairo::RefPtr<Cairo::Context>& cr) const;
        inline void drawName(const Cairo::RefPtr<Cairo::Context>& cr) const;

        GraphViewer* m_parent;

        Glib::ustring m_name;
        int m_x, m_y, m_offset_x = 0, m_offset_y = 0;
        int m_width = 160, m_height = 80;
        double m_height_by_width = 0.5;

        int m_inputs = 2, m_outputs = 1;

        sigc::slot<void(void)> update_callback;
        sigc::slot<void(bool, Node*, int)> link_callback;
};
