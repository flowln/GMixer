#include "MainWindow.h"

#include "HeaderBar.h"

MainWindow::MainWindow()
    : m_main_container()
{ 
    set_title("GMixer");
    set_default_size(900, 450);
    set_titlebar(*HeaderBar::create());

    m_main_container.set_row_homogeneous(true);
    m_main_container.set_column_homogeneous(true);

    m_element_selector = new ElementSelector(this);
    m_pipeline_editor = new PipelineEditor(*this);
    m_pipeline_selector = PipelineSelector::create(this); 

    m_main_container.attach(m_element_selector->the(), 1, 6, 20, 4);
    m_main_container.attach(*m_pipeline_selector->the(), 0, 0, 1, 10);
    m_main_container.attach(*m_pipeline_editor, 1, 0, 20, 6);

    set_child(m_main_container);
}

MainWindow::~MainWindow()
{
}

