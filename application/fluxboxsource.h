#ifndef FLUXBOXSOURCE_H
#define FLUXBOXSOURCE_H

#include <QString>
#include <QDir>
#include <QMap>

class FluxboxSource
{
public:
    struct Files
    {
        QString keys;
    };
    static QString pathJoin(QString p1, QString p2);
    FluxboxSource(QString dir = QString());
    void write();
    QString& operator[](QString name);
private:
    QString read(QString name) const;
    QString m_dir;
    QMap<QString, QString> m_files;
};

#endif // FLUXBOXSOURCE_H
