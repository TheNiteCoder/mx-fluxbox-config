#ifndef MOUSEBUTTONSELECTOR_H
#define MOUSEBUTTONSELECTOR_H

#include <QWidget>
#include <QLabel>

#include "fluxboxkeysequence.h"

class MouseButtonSelector : public QWidget
{
    Q_OBJECT
public:
    explicit MouseButtonSelector(QWidget *parent = nullptr);

    void timerEvent(QTimerEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
signals:
public slots:
    void reset();
private:
    void updateDisplay();
    QLabel* m_infoLabel;
    int m_waitingTimer;
    FluxboxMouseButton m_button;
    FluxboxMouseEventType m_eventType;
};

#endif // MOUSEBUTTONSELECTOR_H
