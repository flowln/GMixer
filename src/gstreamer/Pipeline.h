#pragma once

#include <gst/gst.h>
#include <glibmm/object.h>
#include <glibmm/ustring.h>

// Forward-declaration
class Element;
class PipelineFactory;

class Pipeline final : public Glib::Object {
    friend class PipelineFactory;
    using element_iter_ptr = std::unique_ptr<GstIterator, GstIteratorFreeFunction>;

    public:
        /** Adds element to the pipeline.
         *  Returns whether adding the element was (un)successful.
         * */
        bool addElement(Element* element);

        guint addWatcher(GstBusFunc c)       { return gst_bus_add_watch(gst_pipeline_get_bus(m_pipeline), c, NULL); }
        void addErrorListener(GCallback c)   { g_signal_connect(gst_pipeline_get_bus(m_pipeline), "message::error", c, NULL); }
        void addWarningListener(GCallback c) { g_signal_connect(gst_pipeline_get_bus(m_pipeline), "message::warning", c, NULL); }
        void addInfoListener(GCallback c)    { g_signal_connect(gst_pipeline_get_bus(m_pipeline), "message::info", c, NULL); }

        enum State {
            VOID_PENDING = GST_STATE_VOID_PENDING,
            NIL          = GST_STATE_NULL,
            READY        = GST_STATE_READY,
            PAUSED       = GST_STATE_PAUSED,
            PLAYING      = GST_STATE_PLAYING
        };
        enum StateChangeResult {
            FAILURE      = GST_STATE_CHANGE_FAILURE,
            SUCCESS      = GST_STATE_CHANGE_SUCCESS,
            ASYNC        = GST_STATE_CHANGE_ASYNC,
            NO_PREROLL   = GST_STATE_CHANGE_NO_PREROLL
        };
        StateChangeResult changeState(State new_state);
        State             currentState() const;

        element_iter_ptr getElementsSorted() { return element_iter_ptr(gst_bin_iterate_sorted(GST_BIN( m_pipeline )), gst_iterator_free); }
        Glib::ustring getName() const { return m_name; }
        const gchar* getCommand() const;

        bool isEmpty() const { return GST_BIN_NUMCHILDREN( m_pipeline ) == 0; }

    protected:
        Pipeline(Glib::ustring&);
        Pipeline(GstPipeline*);
        Pipeline(Glib::ustring, GstPipeline*);

        ~Pipeline();

    private:
        Glib::ustring m_name = { "" };
        GstPipeline* m_pipeline;
};

class PipelineFactory final{
    public:
        static Pipeline* createEmpty(Glib::ustring& name);
        static Pipeline* createFromString(Glib::ustring&& name, const gchar* repr);
        static Pipeline* wrapBasePipeline(GstPipeline*);

        static gboolean defaultMessageHandler(GstBus*, GstMessage*, gpointer);
};

