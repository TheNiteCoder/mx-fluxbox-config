#include "tab.h"

Tab::Tab(const QString name, const QIcon icon, QObject *parent)
    : QObject(parent)
{
    setName(name);
    setIcon(icon);
}

Tab::Tab(Tab &copy)
    : QObject(copy.parent())
{
    *this = copy;
}

Tab::Tab(Tab &&move)
    : QObject(move.parent())
{
    *this = move;
}

Tab& Tab::operator =(Tab &copy)
{
    setName(copy.name());
    setIcon(copy.icon());
    return *this;
}

Tab& Tab::operator =(Tab &&move)
{
    setName(move.name());
    setIcon(move.icon());
    move.setIcon(QIcon());
    move.setName(QString());
    return *this;
}

Tab::~Tab()
{
}

QString Tab::name()
{
    return m_name;
}

QIcon Tab::icon()
{
    return m_icon;
}

Tab &Tab::setWidget(QWidget *widget)
{
    m_widget = widget;
    return *this;
}

QWidget *Tab::widget()
{
    return m_widget;
}

Tab& Tab::setIcon(const QIcon icon)
{
    m_icon = icon;
    return *this;
}

Tab& Tab::setName(const QString name)
{
    m_name = name;
    return *this;
}

TabManager::TabManager(QList<Tab*> tabs, QTabWidget *tabWidget, QObject *parent)
    : QObject(parent), m_tabs(tabs)
{
    if(tabWidget) setupTabWidget(tabWidget);
}

void TabManager::registerTab(Tab *tab)
{
    m_tabs.append(tab);
}

void TabManager::setup(FluxboxSource source)
{
    for(Tab* tab : m_tabs)
    {
        tab->setup(source);
    }
}

void TabManager::apply(FluxboxSource& source)
{
    for(Tab* tab : m_tabs)
    {
        tab->apply(source);
    }
}

void TabManager::setupTabWidget(QTabWidget *tabWidget)
{
    for(Tab* tab : m_tabs)
    {
        Q_ASSERT(tab->widget() != nullptr);
        tabWidget->addTab(tab->widget(), tab->icon(), tab->name());
    }
}
