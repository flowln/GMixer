#pragma once

#include <cairomm/cairomm.h>
#include <gst/gstpad.h>

class Node;

class Pad {
   public:
    enum class Availability { ALWAYS = 0, SOMETIMES = 1, ON_DEMAND = 2 };

    Pad(Node* parent, GstPad* base, Availability aval = Availability::ALWAYS);
    virtual ~Pad() = default;

    void destroy();

    bool operator==(GstPad*);
    GstPad* getBase() const { return m_base; }

    void draw(const Cairo::RefPtr<Cairo::Context>& cr, bool is_mouse_over = false);

    void setPosition(double x, double y)
    {
        m_x = x;
        m_y = y;
    }
    void setSize(double w, double h)
    {
        m_width  = w;
        m_height = h;
    }
    double lastX() const { return m_x; }
    double lastY() const { return m_y; }
    double lastWidth() const { return m_width; }
    double lastHeight() const { return m_height; }
    bool contains(double x, double y) const;

    void stageLinking() { m_staging_link = true; }
    void cancelLinking() { m_staging_link = false; }
    bool lookingForLink() const { return m_staging_link; }
    bool isLinked() const { return m_peer != nullptr; }
    bool isOutdated() const { return !isLinked() && m_base && gst_pad_is_linked(m_base); }

    virtual void link(Pad* peer, bool initiator = true){};
    virtual void unlink(bool initiator = true){};

    Pad* getPeer() { return m_peer; };
    Pad* getPeerOfBase();

   protected:
    double m_x = 0, m_y = 0;
    double m_width = 0, m_height = 0;
    bool m_staging_link = false;
    Availability m_availability;

    Node* m_parent;
    GstPad* m_base;

    Pad* m_peer      = nullptr;
    bool m_initiator = true;
};

class InputPad : public Pad {
   public:
    InputPad(Node* parent, GstPad* base, Availability aval = Availability::ALWAYS) : Pad(parent, base, aval){};
    virtual void link(Pad* peer, bool initiator = true) override;
    virtual void unlink(bool initiator = true) override;
};
class OutputPad : public Pad {
   public:
    OutputPad(Node* parent, GstPad* base, Availability aval = Availability::ALWAYS) : Pad(parent, base, aval){};
    virtual void link(Pad* peer, bool initiator = true) override;
    virtual void unlink(bool initiator = true) override;
};
