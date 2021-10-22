#include "MainWindow.h"

MainWindow::MainWindow()
    : m_main_container()
{ 
    set_title("GMixer");
    set_default_size(900, 450);

    auto es = Gtk::make_managed<ElementSelector>();
    m_ps = new PipelineSelector(this); 
    
    m_main_container.attach(*es, 1, 1, 1, 1);
    m_main_container.attach(*m_ps->the(), 0, 0, 1, 3);

    set_child(m_main_container);
}

MainWindow::~MainWindow()
{
    delete m_ps;
}

