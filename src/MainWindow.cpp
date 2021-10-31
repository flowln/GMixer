#include "MainWindow.h"

MainWindow::MainWindow()
    : m_main_container()
{ 
    set_title("GMixer");
    set_default_size(900, 450);

    m_main_container.set_row_homogeneous(true);
    m_main_container.set_column_homogeneous(true);

    m_es = new ElementSelector(this);
    m_ps = new PipelineSelector(this); 

    m_main_container.attach(*m_ps->the(), 0, 0, 1, 10);
    m_main_container.attach(*m_es->the(), 1, 6, 20, 4);

    set_child(m_main_container);
}

MainWindow::~MainWindow()
{
}

