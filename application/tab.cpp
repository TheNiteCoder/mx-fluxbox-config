#include "tab.h"

Tab::Tab(QString name, QIcon icon)
    : m_widget(nullptr), m_name(name), m_icon(icon)
{
}

Tab::~Tab()
{
    delete m_widget;
}

QWidget *Tab::widget()
{
    return m_widget;
}

void Tab::setWidget(QWidget *widget)
{
    m_widget = widget;
}

QString Tab::name() const
{
    return m_name;
}

QIcon Tab::icon() const
{
    return m_icon;
}
