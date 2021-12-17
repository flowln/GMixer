#include "MainWindow.h"

#include "HeaderBar.h"

#include "views/ElementSelector.h"
#include "views/PipelineEditor.h"
#include "views/PipelineSelector.h"

MainWindow::MainWindow()
    : m_main_container()
{ 
    set_title("GMixer");
    set_default_size(900, 450);
    set_titlebar(*HeaderBar::create());

    m_main_container.set_row_homogeneous(true);
    m_main_container.set_column_homogeneous(true);

    auto element_selector = ElementSelector::create(this);
    auto pipeline_selector = PipelineSelector::create(this); 

    PipelineSelector::signal_pipeline_selected.connect(
        [this](Pipeline* selected){
            for(auto editor : m_editors){
                if(editor->getPipeline() == selected){
                    attachEditor(*editor);
                    return;
                }
            }
            auto editor = new PipelineEditor(*this, selected);
            m_editors.push_back(editor);
            attachEditor(*editor);
        });

    m_main_container.attach(element_selector->the(), 1, 6, 20, 4);
    m_main_container.attach(pipeline_selector->the(), 0, 0, 1, 10);

    set_child(m_main_container);
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
    m_main_container.attach(editor, 1, 0, 20, 6);
}

