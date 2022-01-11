#include "HeaderBar.h"

#include "backend/FileUtils.h"
#include "backend/PipelineIO.h"
#include "backend/PipelineListModel.h"
#include "signals/Pipelines.h"

#include <sigc++/functors/ptr_fun.h>

HeaderBar* HeaderBar::s_instance = nullptr;

Gtk::HeaderBar* HeaderBar::create()
{
    if(s_instance)
        return s_instance->m_bar;

    s_instance = new HeaderBar();
    return s_instance->m_bar;
}

void HeaderBar::setTitleText(const Glib::ustring text)
{
    if(!s_instance)
        return;
    s_instance->m_title->set_text(text);
}

HeaderBar::HeaderBar()
{
    m_title = Gtk::make_managed<Gtk::Label>();

    m_add_button = Gtk::make_managed<Gtk::Button>();
    m_add_button->set_image_from_icon_name("list-add-symbolic");
    m_add_button->signal_clicked().connect(sigc::ptr_fun(PipelineFactory::createPipeline));

    m_import_button = Gtk::make_managed<Gtk::Button>();
    m_import_button->set_image_from_icon_name("document-open-symbolic");
    m_import_button->signal_clicked().connect(sigc::ptr_fun(PipelineFactory::createPipelineFromFile));

    m_save_button = Gtk::make_managed<Gtk::Button>();
    m_save_button->set_image_from_icon_name("document-save-symbolic");
    m_save_button->signal_clicked().connect(sigc::ptr_fun(PipelineSaver::saveCurrentPipeline));

    m_bar = Gtk::make_managed<Gtk::HeaderBar>();
    m_bar->set_title_widget(*m_title);
    m_bar->pack_start(*m_add_button);
    m_bar->pack_start(*m_import_button);
    m_bar->pack_start(*m_save_button);

    Signals::pipeline_selected().connect(
        [this](Pipeline* selected){
            setTitleText(selected->getName());
        });
}

