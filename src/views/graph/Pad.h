#pragma once

#include <gst/gstpad.h>
#include <cairomm/cairomm.h>

class Node;

class Pad {
    public:
        Pad(Node* parent, GstPad* base);
        virtual ~Pad() = default;

        bool operator==(GstPad*);
        GstPad* getBase() const { return m_base; }

        void draw(const Cairo::RefPtr<Cairo::Context>& cr);

        void setPosition(double x, double y) { m_last_x = x; m_last_y = y; }
        void setSize(double w, double h) { m_last_width = w; m_last_height = h; }
        double lastX() const { return m_last_x; }
        double lastY() const { return m_last_y; }

        void stageLinking() { m_staging_link = true; }
        void cancelLinking() { m_staging_link = false; }
        bool lookingForLink() const { return m_staging_link; }
        bool isLinked() const { return m_peer != nullptr; }
        bool isOutdated() const { return !isLinked() && gst_pad_is_linked(m_base); } 

        virtual void link(Pad* peer, bool initiator = true) {};
        virtual void unlink(bool initiator = true) {};

        Pad* getPeerOfBase();

    protected:
        double m_last_x = 0, m_last_y = 0;
        double m_last_width = 0, m_last_height = 0;
        bool m_staging_link = false;

        Node* m_parent;
        GstPad* m_base;

        Pad* m_peer = nullptr;
        bool m_initiator = true;
};

class InputPad : public Pad {
    public: 
        InputPad(Node* parent, GstPad* base) : Pad(parent, base){};
        virtual void link(Pad* peer, bool initiator = true) override;
        virtual void unlink(bool initiator = true) override;
};
class OutputPad : public Pad {
    public:
        OutputPad(Node* parent, GstPad* base) : Pad(parent, base){};
        virtual void link(Pad* peer, bool initiator = true) override;
        virtual void unlink(bool initiator = true) override;
};

