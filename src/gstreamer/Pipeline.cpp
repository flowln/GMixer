#include "gstreamer/Element.h"
#include "gstreamer/ElementUtils.h"
#include "gstreamer/Pipeline.h"

Pipeline::Pipeline(Glib::ustring& name)
    : Pipeline(name, GST_PIPELINE( gst_pipeline_new(name.c_str()) ))
{}

Pipeline::Pipeline(GstPipeline* gst_pipeline)
    : Pipeline(gst_element_get_name(gst_pipeline), gst_pipeline)
{}

Pipeline::Pipeline(Glib::ustring name, GstPipeline* gst_pipeline)
    : m_name(name)
    , m_pipeline(gst_pipeline)
{}

Pipeline::~Pipeline()
{
    gst_object_unref(m_pipeline);
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

bool Pipeline::addElement(Element* element)
{
    if(!element)
        return false;

    return gst_bin_add(GST_BIN ( m_pipeline ), element->getBase());
}

inline GstStateChange getChange(GstState bef, Pipeline::State aft)
{
    return GstStateChange((bef << 3) | aft);
}

Pipeline::StateChangeResult Pipeline::changeState(Pipeline::State new_state)
{
    return static_cast<StateChangeResult>(gst_element_set_state(GST_ELEMENT(m_pipeline), static_cast<GstState>(new_state)));
}

Pipeline::State Pipeline::currentState() const
{
    return static_cast<State>(GST_STATE(m_pipeline));
}

// Pipeline Factory

Pipeline* PipelineFactory::createEmpty(Glib::ustring& name)
{
    auto pipeline = new Pipeline(name);
    pipeline->addWatcher(&PipelineFactory::defaultMessageHandler );
    return pipeline;
}

Pipeline* PipelineFactory::createFromString(Glib::ustring&& name, const gchar* repr)
{
    auto context = gst_parse_context_new();
    GError* error = nullptr;
    
    auto gst_pipeline = gst_parse_launch_full(repr, context, GST_PARSE_FLAG_NONE, &error);
    if(!gst_pipeline){ // Could not create bin
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

    gst_element_set_name(gst_pipeline, std::move(name.c_str()));
    auto pipeline = new Pipeline(name, GST_PIPELINE( gst_pipeline )); 
    pipeline->addWatcher(&PipelineFactory::defaultMessageHandler );
    return pipeline;
}

Pipeline* PipelineFactory::wrapBasePipeline(GstPipeline* gst_pipeline)
{
    auto pipeline = new Pipeline(gst_pipeline);
    pipeline->addWatcher(&PipelineFactory::defaultMessageHandler);
    return pipeline;
}

gboolean PipelineFactory::defaultMessageHandler(GstBus* bus, GstMessage* message, gpointer ptr)
{
    gchar*  debug;
    GError* error;
    
    switch(GST_MESSAGE_TYPE(message)){
    case GST_MESSAGE_ERROR: 
        gst_message_parse_error (message, &error, &debug);

        g_printerr ("Error: %s\n", error->message);
        g_printerr ("\tDebugging info: %s\n", (debug) ? debug : "none");

        g_error_free (error);
        g_free (debug);

        break;
    case GST_MESSAGE_WARNING:
        gst_message_parse_warning (message, &error, &debug);
        g_free (debug);

        g_print ("Warning: %s\n", error->message);
        g_error_free (error);

        break;
    case GST_MESSAGE_INFO:
        gst_message_parse_info (message, &error, &debug);
        g_free (debug);

        g_print ("Info: %s\n", error->message);
        g_error_free (error);

        break;
    case GST_MESSAGE_STATE_CHANGED:
        GstState old_state, new_state;

        gst_message_parse_state_changed (message, &old_state, &new_state, NULL);

        break;
    default:
        break;
    }
    return true;
}
