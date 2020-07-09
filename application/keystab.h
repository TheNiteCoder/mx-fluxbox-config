#ifndef KEYSTAB_H
#define KEYSTAB_H

#include "tab.h"

#include <QKeySequence>

namespace Ui
{
class KeysTab;
}

class KeysTab : public Tab
{
public:
    KeysTab();
    void setup(FluxboxSource::Files source);
    void apply(FluxboxSource::Files& source);
private:
    static QList<QKeySequence> fromString(QStringList keys);
    static QStringList toString(QKeySequence sequence);
    static Qt::Key maptoKey(QString str);
    static Qt::Modifier mapToModifier(QString str);
    Ui::KeysTab* ui;
};

#endif // KEYSTAB_H
