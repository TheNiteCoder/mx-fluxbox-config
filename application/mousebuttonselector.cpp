#include "mousebuttonselector.h"

#include <QHBoxLayout>
#include <QTimerEvent>
#include <QTimer>
#include <QMouseEvent>

MouseButtonSelector::MouseButtonSelector(QWidget *parent)
    : QWidget(parent), m_infoLabel(new QLabel), m_waitingTimer(-1)
{
    setLayout(new QHBoxLayout);
    layout()->addWidget(m_infoLabel);
    reset();
}

void MouseButtonSelector::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == m_waitingTimer)
    {
        // if timer runs out before mouse movement or release it becomes a Mouse(Press)
        m_eventType = FluxboxMouseEventType::Press;
        m_waitingTimer = -1;
    }
    updateDisplay();
}

void MouseButtonSelector::mousePressEvent(QMouseEvent *e)
{
    reset();
    m_button = FluxboxKeySequence::toFluxboxMouseButton(e->button());
    m_waitingTimer = startTimer(1500);
    updateDisplay();
}

void MouseButtonSelector::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    if(m_waitingTimer < 0)
        return;
    m_eventType = FluxboxMouseEventType::Click;
    updateDisplay();
}

void MouseButtonSelector::mouseDoubleClickEvent(QMouseEvent *e)
{
    killTimer(m_waitingTimer);
    m_waitingTimer = -1;
    m_eventType = FluxboxMouseEventType::DoubleClick;
    m_button = FluxboxKeySequence::toFluxboxMouseButton(e->button());
    updateDisplay();
}

void MouseButtonSelector::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    if(m_waitingTimer < 0)
        return;
    killTimer(m_waitingTimer);
    m_waitingTimer = -1;
    m_eventType = FluxboxMouseEventType::Move;
    updateDisplay();
}

void MouseButtonSelector::reset()
{
    if(m_waitingTimer > -1) killTimer(m_waitingTimer);
    m_waitingTimer = -1;
    m_eventType = FluxboxMouseEventType::Unknown;
    m_button = FluxboxMouseButton::ButtonUnknown;
    updateDisplay();
}

void MouseButtonSelector::updateDisplay()
{
    QString result;
    if(m_eventType == FluxboxMouseEventType::Unknown &&
            m_button == FluxboxMouseButton::ButtonUnknown)
    {
        result = "Empty";
    }
    else
    {
        QStringList parts;
        switch(m_eventType)
        {
#define C(t) case FluxboxMouseEventType::t: parts << #t; break
        C(Unknown);
        C(Move);
        C(DoubleClick);
        C(Press);
        C(Click);
        }
#undef C
#define C(t) case FluxboxMouseButton::t: parts << #t; break
        switch(m_button)
        {
        C(Button1);
        C(Button2);
        C(Button3);
        C(Button4);
        C(Button5);
        C(ButtonUnknown);
        }
#undef C
        result = parts.join(' ');
    }
    m_infoLabel->setText(result);
}
