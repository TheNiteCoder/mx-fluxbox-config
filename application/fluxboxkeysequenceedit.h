#ifndef FLUXBOXKEYSEQUENCEEDIT_H
#define FLUXBOXKEYSEQUENCEEDIT_H

#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include "fluxboxkeysequence.h"

class FluxboxKeySequenceEdit : public QLineEdit
{
    Q_OBJECT
public:
    FluxboxKeySequenceEdit(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* e);
    // void keyReleaseEvent(QKeyEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void timerEvent(QTimerEvent* e);
    bool event(QEvent* e);
    FluxboxKeySequence keySequence() const { return m_sequence; }
    void setKeySequence(FluxboxKeySequence sequence) { m_sequence = sequence; emit keySequenceChanged(); }
signals:
    void editingFinished();
    void keySequenceChanged();
public slots:
    void reset();
private slots:
    void resetState();
    void updateDisplay();
private:
    void stopTimer(int& id);
    int m_timeoutTimer;
    int m_waitForPressTimer;
    static bool isModifier(Qt::Key key);
    FluxboxKeySequence m_sequence;
};

class FluxboxKeySequenceEditWithMouseOptions : public QWidget
{
    Q_OBJECT
public:
    FluxboxKeySequenceEditWithMouseOptions(QWidget* parent = nullptr);
    void setKeySequence(FluxboxKeySequence seq) { m_editor->setKeySequence(seq); }
    FluxboxKeySequence keySequence() const { return m_editor->keySequence(); }
private slots:
    void launchExtraSettings();
signals:
    void editingFinished();
    void keySequenceChanged();
private:
    FluxboxKeySequenceEdit* m_editor;
    QPushButton* m_extraSettings;
    QPushButton* m_reset;
};

#endif // FLUXBOXKEYSEQUENCEEDIT_H
