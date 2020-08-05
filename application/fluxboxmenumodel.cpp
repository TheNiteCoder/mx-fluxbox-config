#include "fluxboxmenumodel.h"

#include <QRegularExpression>

FluxboxMenuModel::FluxboxMenuModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Tree";
    m_rootItem = new FluxboxMenuItem(rootData);
    QStringList lines = data.split('\n');
    setupTree(lines, m_rootItem);
}

FluxboxMenuModel::~FluxboxMenuModel()
{
    delete m_rootItem;
}

QVariant FluxboxMenuModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return {};
    if(role != Qt::DisplayRole)
        return {};

    FluxboxMenuItem* item = static_cast<FluxboxMenuItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags FluxboxMenuModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemFlag::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant FluxboxMenuModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data(section);
    return {};
}

QModelIndex FluxboxMenuModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return {};

    FluxboxMenuItem* parentItem;

    if(!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<FluxboxMenuItem*>(parent.internalPointer());

    FluxboxMenuItem* childItem = parentItem->child(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return {};
}

QModelIndex FluxboxMenuModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return {};

    FluxboxMenuItem* childItem = static_cast<FluxboxMenuItem*>(index.internalPointer());
    FluxboxMenuItem* parentItem = childItem->parent();

    if(parentItem == m_rootItem)
        return {};

    return createIndex(parentItem->row(), 0, parentItem);
}

int FluxboxMenuModel::rowCount(const QModelIndex &parent) const
{
    if(parent.column() > 0)
        return 0;

    FluxboxMenuItem* parentItem;
    if(!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<FluxboxMenuItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int FluxboxMenuModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return static_cast<FluxboxMenuItem*>(parent.internalPointer())->columnCount();
    else
        return m_rootItem->columnCount();
}

void FluxboxMenuModel::setupTree(QStringList &lines, FluxboxMenuItem *parent, QString end)
{
    Tag tag;
    while(!tag.isValid())
    {
        tag = parseTag(lines.first());
        lines.removeFirst();
    }
}

FluxboxMenuModel::Tag FluxboxMenuModel::parseTag(QString line)
{
    Tag result;
    while(true)
    {
        line = line.trimmed();
        if(line.size() == 0) break;
#define P(b, e, m) if(line[0] == b) {\
    int end = line.indexOf(e);\
    if(end == -1) break;\
    QString value = line.mid(1, end - 2);\
    result.m = value;\
    line = line.mid(end + 1);}
        P('[', ']', squareBrackets)
        else P('{', '}', curlyBrackets)
        else P('<', '>', angleBrackets)
        else P('(', ')', parentheses)
        else break;
    }
    return result;
}


bool FluxboxMenuModel::Tag::isValid() const
{
    return !(parentheses.isEmpty() && curlyBrackets.isEmpty() &&
             squareBrackets.isEmpty() && angleBrackets.isEmpty());
}
