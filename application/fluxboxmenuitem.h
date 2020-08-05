#ifndef FLUXBOXMENUITEM_H
#define FLUXBOXMENUITEM_H

#include <QList>
#include <QVariant>

class FluxboxMenuItem
{
public:
    FluxboxMenuItem(FluxboxMenuItem* parent = nullptr);
    explicit FluxboxMenuItem(const QList<QVariant>& data, FluxboxMenuItem* parent = nullptr);
    ~FluxboxMenuItem();

    void appendChild(FluxboxMenuItem* child);

    FluxboxMenuItem* child(int row);
    int childCount() const { return m_childItems.size(); }
    int columnCount() const { return m_columnData.size(); }
    QVariant data(int column) const { return m_columnData.value(column); }
    int row() const;
    FluxboxMenuItem* parent() { return m_parentItem; }
private:
    QList<FluxboxMenuItem*> m_childItems;
    QList<QVariant> m_columnData;
    FluxboxMenuItem* m_parentItem;
};

Q_DECLARE_METATYPE(FluxboxMenuItem)

#endif // FLUXBOXMENUITEM_H
