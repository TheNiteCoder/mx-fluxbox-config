#include "window.h"
#include "ui_window.h"

#include "keystab.h"
#include "menutab.h"

#include <QSettings>

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
    setWindowTitle(NAME);
    readSettings();
    connect(ui->pushButton_Close, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->pushButton_Apply, &QPushButton::clicked, [this](){
        FluxboxSource source = m_fluxboxSource;
        m_tabManager.apply(source);
        source.write();
    });
    m_tabManager.registerTab(new KeysTab);
    m_tabManager.registerTab(new MenuTab);
    m_tabManager.setupTabWidget(ui->tabWidget_Tabs);
    FluxboxSource source = m_fluxboxSource;
    m_tabManager.setup(source);
}

Window::~Window()
{
    delete ui;
}

void Window::closeEvent(QCloseEvent *e)
{
    writeSettings();
    e->accept();
}

void Window::readSettings()
{
    QSettings settings{ORG, NAME};

    settings.beginGroup("window");

    restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
    resize(settings.value("size", size()).toSize());
    if(settings.value("maximized", isMaximized()).toBool())
        showMaximized();

    settings.endGroup();
}

void Window::writeSettings()
{
    QSettings settings{ORG, NAME};

    settings.beginGroup("window");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("maximized", isMaximized());
    if(!isMaximized())
        settings.setValue("size", size());

    settings.endGroup();
}
