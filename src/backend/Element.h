#pragma once

#include <glibmm/object.h>
#include <gst/gst.h>

class Element : public Glib::Object{
    public:
        Element(const gchar* name);
        Element(GstElement*);
        ~Element();

        const gchar* getName() const;

        void addSink(GstPad* pad) { m_sinks.push_front(pad); }
        void addSource(GstPad* pad) { m_sources.push_front(pad); }

        std::list<GstPad*>& getSinks() { return m_sinks; }
        std::list<GstPad*>& getSources() { return m_sources; }

        int getNumSinks() const { return m_sinks.size(); }
        int getNumSources() const { return m_sources.size(); }

        int getIndexOfSink(GstPad*) const;
        int getIndexOfSource(GstPad*) const;

        static int isPadLinked(GstPad*);

    private:
        GstElement* m_element;

        static gboolean countPads(GstElement*, GstPad*, gpointer instance);
        std::list<GstPad*> m_sinks;
        std::list<GstPad*> m_sources;

};
