#pragma once

#include <glibmm/object.h>
#include <gst/gst.h>

class Element : public Glib::Object{
    public:
        Element(const gchar* name);
        Element(GstElement*);
        ~Element();

        bool operator==(Element* elem) const { return m_element == elem->getBase(); };
        bool operator==(GstElement* elem) const { return m_element == elem; };

        const gchar* getName() const;
        GstElement* getBase() const { return m_element; }
        GstElement* getPeer() const { return GST_ELEMENT( gst_pad_get_parent(GST_PAD_PEER(m_element)) ); }

        GList* getSinks() const;
        GList* getSources() const;

        static int isPadLinked(GstPad*);

    private:
        GstElement* m_element;

};
