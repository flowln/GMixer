#include "client/gtk/HeaderBar.h"

#include "client/gtk/Dialogs.h"
#include "client/gtk/PipelineListModel.h"
#include "client/gtk/PipelineSelector.h"
#include "signals/Pipelines.h"

#include <sigc++/functors/ptr_fun.h>

#define MEDIA_RESUME_ICON_NAME "media-playback-start-symbolic"
#define MEDIA_PAUSE_ICON_NAME  "media-playback-pause-symbolic"
#define MEDIA_STOP_ICON_NAME   "media-playback-stop-symbolic"

HeaderBar* HeaderBar::s_instance = nullptr;

Gtk::HeaderBar* HeaderBar::create()
{
    if (s_instance)
        return s_instance->m_bar;

    s_instance = new HeaderBar();
    return s_instance->m_bar;
}

void HeaderBar::setTitleText(const Glib::ustring text)
{
    if (!s_instance)
        return;
    s_instance->m_title->set_text(text);
}

HeaderBar::HeaderBar()
{
    // Create widgets
    m_title = Gtk::make_managed<Gtk::Label>();

    m_add_button = Gtk::make_managed<Gtk::Button>();
    m_add_button->set_image_from_icon_name("list-add-symbolic");
    m_add_button->signal_clicked().connect(sigc::ptr_fun(Dialog::createPipeline));

    m_import_button = Gtk::make_managed<Gtk::Button>();
    m_import_button->set_image_from_icon_name("document-open-symbolic");
    m_import_button->signal_clicked().connect(sigc::ptr_fun(Dialog::importPipelineFromFile));

    m_save_button = Gtk::make_managed<Gtk::Button>();
    m_save_button->set_image_from_icon_name("document-save-symbolic");
    // m_save_button->signal_clicked().connect(sigc::ptr_fun(PipelineSaver::saveCurrentPipeline));

    /*
    m_control_pipeline = Gtk::make_managed<Gtk::Button>();
    m_control_pipeline->set_image_from_icon_name(MEDIA_RESUME_ICON_NAME);
    m_control_pipeline->signal_clicked().connect([&] {
        static bool is_playing = false;
        if (is_playing) {
            if (PipelineSelector::currentPipeline()->changeState(Pipeline::State::PAUSED))
                m_control_pipeline->set_image_from_icon_name(MEDIA_RESUME_ICON_NAME);
        } else {
            if (PipelineSelector::currentPipeline()->changeState(Pipeline::State::PLAYING))
                m_control_pipeline->set_image_from_icon_name(MEDIA_PAUSE_ICON_NAME);
        }
        is_playing = !is_playing;
    });
    */

    m_bar = Gtk::make_managed<Gtk::HeaderBar>();
    m_bar->set_title_widget(*m_title);
    m_bar->pack_start(*m_add_button);
    m_bar->pack_start(*m_import_button);
    m_bar->pack_start(*m_save_button);
    // m_bar->pack_start(*m_control_pipeline);

    Signals::pipeline_selected().connect([this](Pipeline* selected) { setTitleText(selected->getName()); });
}
