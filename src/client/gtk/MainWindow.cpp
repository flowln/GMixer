#include "client/gtk/MainWindow.h"

#include "client/gtk/Dialogs.h"
#include "client/gtk/HeaderBar.h"
#include "client/gtk/ElementSelector.h"
#include "client/gtk/PipelineEditor.h"
#include "client/gtk/PipelineSelector.h"

#include "signals/Pipelines.h"

static void changePipelineEditor(MainWindow* window, Pipeline* selected)
{
    auto& editors = window->getEditors();

    // Search for an editor connected to the selected pipeline
    for(auto editor : editors){
        if(editor->getPipeline() == selected){
            window->attachEditor(*editor);
            return;
        }
    }

    // Create a new editor for this new pipeline
    auto editor = new PipelineEditor(*window, selected);
    editors.push_back(editor);

    // Select the newly created pipeline's editor
    window->attachEditor(*editor);
}

MainWindow::MainWindow()
    : m_main_container()
{ 
    // Set own attributes
    set_title("GMixer");
    set_size_request(750, 500);
    set_titlebar(*HeaderBar::create());

    // Setup general settings
    // Dialog
    Dialog::setMainWindow(this);
    Dialog::setFactory(new PipelineFactory());

    // Create child widgets
    auto element_selector = ElementSelector::create(this);
    auto pipeline_selector = PipelineSelector::create(this); 

    m_main_container.attach(element_selector->the(), 1, 1, 1, 1);
    m_main_container.attach(pipeline_selector->the(), 0, 0, 1, 2);

    set_child(m_main_container);

    // Setup signals
    Signals::pipeline_selected().connect(
        [this](Pipeline* selected){
            changePipelineEditor(this, selected);
        });
}
MainWindow::~MainWindow()
{
    for(auto editor : m_editors)
        delete editor;
}

void MainWindow::attachEditor(PipelineEditor& editor)
{
    auto last_child = m_main_container.get_child_at(1, 0);
    if(last_child)
        m_main_container.remove(*last_child);
    m_main_container.attach(editor, 1, 0, 1, 1);
}

