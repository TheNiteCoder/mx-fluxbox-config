#include "keystab.h"
#include "ui_keystab.h"

KeysTab::KeysTab()
    : Tab("Keys", QIcon::fromTheme("keyboard"))
{
    setWidget(new QWidget);
    ui->setupUi(widget());
}

void KeysTab::setup(FluxboxSource::Files source)
{
    auto removeComment = [](QString& line) {
        int pos = (line.contains('#') ? line.indexOf('#') : line.indexOf('!'));
        if(pos < 0) return;
        line = line.mid(0, pos - 1);
    };
    for(QString line : source.keys.split('\n'))
    {
        removeComment(line);
        QStringList keys;
        while(true)
        {
            line = line.trimmed();
            if(line.size() > 0 && line.at(0) == ':')
            {
                break;
            }
            int end = line.indexOf(' ');
            if(end < 0)
                keys << line.mid(0, end);
            else
                keys << line.mid(0);
        }
        QList<QKeySequence> seqs = fromString(keys);
    }
}

void KeysTab::apply(FluxboxSource::Files &source)
{
}

QList<QKeySequence> KeysTab::fromString(QStringList keys)
{
    QList<QKeySequence> sequences;
    while(true)
    {
        if(keys.size() < 2) break;
        QString mod = keys[0];
        QString key = keys[1];
        QKeySequence seq{mod + '+' + key};
        sequences << seq;
    }
    return sequences;
}

Qt::Key KeysTab::maptoKey(QString str)
{
    return Qt::Key(QKeySequence(str)[0]);
}

Qt::Modifier KeysTab::mapToModifier(QString str)
{
    return Qt::Modifier(QKeySequence(str)[0]);
}


