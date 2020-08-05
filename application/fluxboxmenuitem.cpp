#include "fluxboxmenuitem.h"

FluxboxMenuItem::FluxboxMenuItem(FluxboxMenuItem *parent)
{
    m_parentItem = parent;
}

FluxboxMenuItem::FluxboxMenuItem(const QList<QVariant> &data, FluxboxMenuItem *parent)
{
    m_columnData = data;
    m_parentItem = parent;
}

FluxboxMenuItem::~FluxboxMenuItem()
{
    qDeleteAll(m_childItems);
}

void FluxboxMenuItem::appendChild(FluxboxMenuItem *child)
{
    m_childItems.append(child);
}

FluxboxMenuItem *FluxboxMenuItem::child(int row)
{
    return m_childItems.value(row);
}

int FluxboxMenuItem::row() const
{
    if(m_parentItem)
        m_parentItem->m_childItems.indexOf(const_cast<FluxboxMenuItem*>(this));
    return 0;
}


