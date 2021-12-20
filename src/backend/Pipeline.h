#pragma once

#include <gst/gst.h>
#include <glibmm/object.h>
#include <glibmm/ustring.h>

// Forward-declaration
class Element;

class Pipeline : public Glib::Object {
    public:
        Pipeline(Glib::ustring name);
        Pipeline(GstPipeline* gst_pipeline);
        ~Pipeline();

        static Pipeline* createFromString(const gchar* name, const gchar* repr);

        static gboolean handleErrorMessage(GstBus*, GstMessage*, gpointer);
        static gboolean handleWarningMessage(GstBus*, GstMessage*, gpointer);
        static gboolean handleInfoMessage(GstBus*, GstMessage*, gpointer);

        Element* createElement(const gchar* name);
        std::unique_ptr<GstIterator, GstIteratorFreeFunction> getElementsSorted();

        Glib::ustring getName() const{ return m_name; }
        Glib::ustring* getNameReference() { return &m_name; }
        const gchar* getCommand() const;

        bool isEmpty() const;

    private:
        Glib::ustring m_name = { "" };
        GstPipeline* m_pipeline;
};

