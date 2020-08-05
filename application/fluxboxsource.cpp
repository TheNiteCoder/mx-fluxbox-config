#include "fluxboxsource.h"

#include <QTextStream>

QString FluxboxSource::pathJoin(QString p1, QString p2)
{
    return QDir::cleanPath(p1 + QDir::separator() + p2);
}

FluxboxSource::FluxboxSource(QString dir)
{
    if(dir.isNull())
        m_dir = pathJoin(QDir::homePath(), ".fluxbox");
    else
        m_dir = dir;
}

void FluxboxSource::write()
{
    for(QString key : m_files.keys())
    {
        QFile file{pathJoin(m_dir, key)};
        if(!file.open(QFile::WriteOnly | QFile::Text))
        {
            // TODO ERROR
        }
        QTextStream stream{&file};
        stream << m_files[key];
        file.close();
    }
}

QString& FluxboxSource::operator[](QString name)
{
    if(m_files.contains(name)) return m_files[name];
    m_files[name] = read(name);
    return m_files[name];
}

QString FluxboxSource::read(QString name) const
{
    QFile file{pathJoin(m_dir, name)};
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        // TODO ERROR
    }
    QString contents = file.readAll();
    file.close();
    return contents;
}


