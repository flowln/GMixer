#pragma once

#include "gst/gst.h"
#include "glib/ghash.h"
#include "glibmm/object.h"
#include "glibmm/ustring.h"

class Pipeline {
    public:
        static std::shared_ptr<Pipeline> create(const Glib::ustring& name);

        ~Pipeline();

        static gboolean handleErrorMessage(GstBus*, GstMessage*, gpointer);
        static gboolean handleWarningMessage(GstBus*, GstMessage*, gpointer);
        static gboolean handleInfoMessage(GstBus*, GstMessage*, gpointer);

        Glib::ustring getName() const{ return m_name; }
    protected:
        Pipeline(Glib::ustring name);

    private:
        Glib::ustring m_name = { "" };
        GstPipeline* m_pipeline;
};

