#ifndef FLUXBOXMENUMODEL_H
#define FLUXBOXMENUMODEL_H

#include <QAbstractItemModel>
#include "fluxboxmenuitem.h"

class FluxboxMenuModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit FluxboxMenuModel(const QString& data, QObject* parent = nullptr);
    ~FluxboxMenuModel();

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
private:
    void setupTree(QStringList& lines, FluxboxMenuItem* parent, QString end = "end");

    struct Tag
    {
        QString squareBrackets;
        QString parentheses;
        QString curlyBrackets;
        QString angleBrackets;
        bool isValid() const;
    };

    Tag parseTag(QString line);

    FluxboxMenuItem* m_rootItem;
};

#endif // FLUXBOXMENUMODEL_H
