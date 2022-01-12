#include "client/MainWindow.h"

#include "client/HeaderBar.h"
#include "client/ElementSelector.h"
#include "client/PipelineEditor.h"
#include "client/PipelineSelector.h"

#include "signals/Pipelines.h"

MainWindow::MainWindow()
    : m_main_container()
{ 
    set_title("GMixer");
    set_size_request(750, 500);
    set_titlebar(*HeaderBar::create());

    //m_main_container.set_row_homogeneous(true);
    //m_main_container.set_column_homogeneous(true);

    auto element_selector = ElementSelector::create(this);
    auto pipeline_selector = PipelineSelector::create(this); 

    Signals::pipeline_selected().connect(
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

    m_main_container.attach(element_selector->the(), 1, 1, 1, 1);
    m_main_container.attach(pipeline_selector->the(), 0, 0, 1, 2);

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
    m_main_container.attach(editor, 1, 0, 1, 1);
}

