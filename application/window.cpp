#include "window.h"
#include "ui_window.h"

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
    m_tabs.push_back(new KeysTab);
    setupTabs();
}

Window::~Window()
{
    delete ui;
}

void Window::setupTabs()
{
    for(Tab* tab : m_tabs)
    {
        ui->tabWidget_Tabs->addTab(tab->widget(), tab->icon(), tab->name());
    }
}
