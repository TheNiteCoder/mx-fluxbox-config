#include "fluxboxsource.h"

FluxboxSource::FluxboxSource(QString dir)
{
    if(dir.isNull())
    {
        dir = path::join(QDir::homePath(), ".fluxbox");
    }
}

QString FluxboxSource::dir() const
{
    return m_dir;
}

void FluxboxSource::setDir(QString dir)
{
    m_dir = dir;
}

FluxboxSource::Files FluxboxSource::read()
{
    Files result;
    QFile keysFile{path::join(m_dir, "keys")};
    if(!keysFile.open(QFile::ReadOnly))
    {
        // ERROR
    }
    result.keys = keysFile.readAll();
    keysFile.close();
    return result;
}

void FluxboxSource::write(FluxboxSource::Files files)
{
    QFile keysFile{path::join(m_dir, "keys")};
    if(!keysFile.open(QFile::WriteOnly))
    {
        // ERROR
    }
    keysFile.write(files.keys.toStdString().data());
    keysFile.close();
}

QString path::join(QString p1, QString p2)
{
    return QDir::cleanPath(p1 + QDir::separator() + p2);
}
