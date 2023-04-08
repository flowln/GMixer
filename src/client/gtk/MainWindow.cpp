#include "client/gtk/MainWindow.h"

#include "client/gtk/Dialogs.h"
#include "client/gtk/ElementSelector.h"
#include "client/gtk/HeaderBar.h"
#include "client/gtk/PipelineEditor.h"
#include "client/gtk/PipelineSelector.h"

#include "signals/Pipelines.h"

#include <gtkmm/grid.h>
#include <gtkmm/paned.h>

void MainWindow::changePipelineEditor(Pipeline* selected)
{
    // Search for an editor connected to the selected pipeline
    auto editor_it = std::find_if(m_editors.cbegin(), m_editors.cend(), [selected](auto const& p) { return p->getPipeline() == selected; });
    if (editor_it != m_editors.cend()) {
        attachEditor(**editor_it);
        return;
    }

    // Create a new editor for this new pipeline
    auto editor = std::make_shared<PipelineEditor>(*this, selected);
    m_editors.push_back(editor);

    // Select the newly created pipeline's editor
    attachEditor(*editor);
}

void MainWindow::attachEditor(PipelineEditor& editor)
{
    m_right_container->set_start_child(editor);
}

MainWindow::MainWindow()
    : m_main_container(new Gtk::Grid), m_right_container(new Gtk::Paned(Gtk::Orientation::VERTICAL))
{
    // Set own attributes
    set_title("GMixer");
    set_titlebar(*HeaderBar::create());

    // Setup general settings
    // Dialog
    Dialog::setMainWindow(this);
    Dialog::setFactory(PipelineFactory::create());

    // Create child widgets
    auto pipeline_selector = PipelineSelector::create(this);
    m_main_container->attach(*pipeline_selector, 0, 0, 1, 1);

    auto element_selector  = ElementSelector::create(this);
    m_right_container->set_end_child(*element_selector);

    m_main_container->attach(*m_right_container, 1, 0, 1, 1);

    set_child(*m_main_container);

    // Setup signals
    Signals::pipeline_selected().connect([this](Pipeline* selected) { changePipelineEditor(selected); });
}
