#include "backend/Pipeline.h"
#include "gst/gstutils.h"

Pipeline::Pipeline(Glib::ustring name)
    : m_name(name)
{
    m_pipeline = GST_PIPELINE( gst_pipeline_new(name.c_str()) );

    GstBus* bus = gst_pipeline_get_bus(m_pipeline); 
    g_signal_connect(bus, "message::error", G_CALLBACK( handleErrorMessage ), NULL);
    g_signal_connect(bus, "message::warning", G_CALLBACK( handleWarningMessage ), NULL);
    g_signal_connect(bus, "message::info", G_CALLBACK( handleInfoMessage ), NULL);
}

Pipeline::Pipeline(GstPipeline* gst_pipeline)
    : m_name(std::move(gst_element_get_name(gst_pipeline)))
    , m_pipeline(gst_pipeline)
{
    GstBus* bus = gst_pipeline_get_bus(m_pipeline); 
    g_signal_connect(bus, "message::error", G_CALLBACK( handleErrorMessage ), NULL);
    g_signal_connect(bus, "message::warning", G_CALLBACK( handleWarningMessage ), NULL);
    g_signal_connect(bus, "message::info", G_CALLBACK( handleInfoMessage ), NULL);
}

Pipeline::~Pipeline()
{
    gst_object_unref(m_pipeline);
}

Pipeline* Pipeline::createFromString(const gchar* name, const gchar* repr)
{
    auto context = gst_parse_context_new();
    GError* error = nullptr;
    
    auto pipeline = gst_parse_launch_full(repr, context, GST_PARSE_FLAG_NONE, &error);
    if(!pipeline){ // Could not create bin
        if(error->code == GST_PARSE_ERROR_NO_SUCH_ELEMENT){
            g_printerr("[ERROR] The following elements could not be found: \n");
            auto missing = gst_parse_context_get_missing_elements(context);

            gchar* element;
            int i = 0;
            while((element = missing[i++])){
                g_printerr("\t%s\n", element);
            }

            g_strfreev(missing);
        }
        else {
            g_printerr("[ERROR] %d\n", error->code); 
        }

        g_error_free(error);
        gst_parse_context_free(context);

        return nullptr;
    }

    gst_parse_context_free(context);

    gst_element_set_name(pipeline, std::move(name));
    return new Pipeline(GST_PIPELINE( pipeline ));
}

const gchar* Pipeline::getCommand() const
{
    if(!m_pipeline)
        return nullptr;

    // TODO: There's no way (it seems) to do it directly via gst...
    // We'll probably have to iterate the pipeline and construct
    // the command manually.

    return nullptr;
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
