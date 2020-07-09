#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "keystab.h"

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

private:
    void setupTabs();
    Ui::Window *ui;
    QList<Tab*> m_tabs;
};

#endif // WINDOW_H
