#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

#include "tab.h"
#include "global.h"

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();
    void closeEvent(QCloseEvent* e);
private:
    void readSettings();
    void writeSettings();
    TabManager m_tabManager;
    Ui::Window *ui;
    FluxboxSource m_fluxboxSource;
};

#endif // WINDOW_H
