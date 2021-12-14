#pragma once

#include <gst/gst.h>
#include <glibmm/object.h>
#include <glibmm/ustring.h>

class Pipeline : public Glib::Object {
    public:
        Pipeline(Glib::ustring name);
        Pipeline(GstPipeline* gst_pipeline);
        ~Pipeline();

        static Pipeline* createFromString(const gchar* name, const gchar* repr);

        static gboolean handleErrorMessage(GstBus*, GstMessage*, gpointer);
        static gboolean handleWarningMessage(GstBus*, GstMessage*, gpointer);
        static gboolean handleInfoMessage(GstBus*, GstMessage*, gpointer);

        Glib::ustring getName() const{ return m_name; }
        Glib::ustring* getNameReference() { return &m_name; }
        const gchar* getCommand() const;
    private:
        Glib::ustring m_name = { "" };
        GstPipeline* m_pipeline;
};

