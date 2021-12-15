#pragma once

#include <glibmm/ustring.h>
#include <cairomm/cairomm.h>

class Node {
    public:
        Node(Glib::ustring name, int x, int y);

        void setPosition(int new_x, int new_y);

        void onUpdateCallback(sigc::slot<void(void)>);
        void onClick(double x, double y);

        void draw(const Cairo::RefPtr<Cairo::Context>& cr) const;
        //virtual void select();

        int getX() const { return m_x; }
        int getY() const { return m_y; }

        bool contains(int x, int y) const;

    protected:
        void setSize(int new_width, int new_height);

        inline void drawInputs(const Cairo::RefPtr<Cairo::Context>& cr) const;
        inline void drawOutputs(const Cairo::RefPtr<Cairo::Context>& cr) const;
        inline void drawName(const Cairo::RefPtr<Cairo::Context>& cr) const;

    private:
        Glib::ustring m_name;
        int m_x, m_y;
        int m_width = 160, m_height = 80;
        double m_height_by_width = 0.5;

        int m_inputs = 2, m_outputs = 1;

        sigc::slot<void(void)> update_callback;
};
