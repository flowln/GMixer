#include "backend/Pipeline.h"
#include "glibmm/refptr.h"
#include <memory>

std::shared_ptr<Pipeline> Pipeline::create(const Glib::ustring& name)
{
    return std::shared_ptr<Pipeline>(new Pipeline(name));
}

Pipeline::Pipeline(Glib::ustring name)
    :m_name(name)
{
    m_pipeline = GST_PIPELINE( gst_pipeline_new(name.c_str()) );

    GstBus* bus = gst_pipeline_get_bus(m_pipeline); 
    g_signal_connect(bus, "message::error", G_CALLBACK( handleErrorMessage ), NULL);
    g_signal_connect(bus, "message::warning", G_CALLBACK( handleWarningMessage ), NULL);
    g_signal_connect(bus, "message::info", G_CALLBACK( handleInfoMessage ), NULL);
}

Pipeline::~Pipeline()
{
    gst_object_unref(m_pipeline);
}

gboolean Pipeline::handleErrorMessage(GstBus* bus, GstMessage* msg, gpointer data)
{
    GError* err;
    gchar* debug;

    gst_message_parse_error(msg, &err, &debug);
    g_printerr("[ERROR] %s\n", err->message);
    g_error_free(err);
    g_free(debug);

    return FALSE;
}
gboolean Pipeline::handleWarningMessage(GstBus* bus, GstMessage* msg, gpointer data)
{

    return TRUE;
}
gboolean Pipeline::handleInfoMessage(GstBus* bus, GstMessage* msg, gpointer data)
{
    return TRUE;
}
