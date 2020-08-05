#ifndef MENUTAB_H
#define MENUTAB_H

#include <QWidget>

#include "tab.h"

#include "mousebuttonselector.h"
#include "fluxboxkeysequenceedit.h"

namespace Ui {
class MenuTab;
}

class MenuTab : public Tab
{
    Q_OBJECT
public:
    explicit MenuTab(QObject* parent = nullptr);
    ~MenuTab();
    void setup(FluxboxSource source) {}
    void apply(FluxboxSource& source) {}

private:
    Ui::MenuTab *ui;
};

#endif // MENUTAB_H
