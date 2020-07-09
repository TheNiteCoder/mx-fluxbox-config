#ifndef FLUXBOXSOURCE_H
#define FLUXBOXSOURCE_H

#include <QString>
#include <QMap>
#include <QDir>

namespace path
{

QString join(QString p1, QString p2);

}

class FluxboxSource
{
public:
    struct Files
    {
        QString keys;
    };
    FluxboxSource(QString dir = QString());
    QString dir() const;
    void setDir(QString dir = QString());
    Files read();
    void write(Files files);
private:
    QString m_dir;
};

#endif // FLUXBOXSOURCE_H
