#include "fluxboxkeysequenceedit.h"

#include <QTimerEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVector>
#include <QCheckBox>
#include "global.h"

FluxboxKeySequenceEdit::FluxboxKeySequenceEdit(QWidget *parent)
    : QLineEdit(parent), m_timeoutTimer(-1), m_waitForPressTimer(-1)
{
    setAutoFillBackground(true); // important so that item delegate painting isn't in the background
    setContextMenuPolicy(Qt::NoContextMenu); // removes right click menu
    connect(this, &FluxboxKeySequenceEdit::editingFinished, [this](){
        emit keySequenceChanged();
    });
    connect(this, &FluxboxKeySequenceEdit::keySequenceChanged, [this](){
        updateDisplay();
    });
}

void FluxboxKeySequenceEdit::keyPressEvent(QKeyEvent *e)
{
    if(isModifier(static_cast<Qt::Key>(e->key())))
    {
        return;
    }
    m_sequence.setModifiers(e->modifiers());
    m_sequence.setKey(static_cast<Qt::Key>(e->key()));
    emit editingFinished();
}

void FluxboxKeySequenceEdit::mousePressEvent(QMouseEvent *e)
{
    m_sequence.setMouseButton(FluxboxKeySequence::toFluxboxMouseButton(e->button()));
    m_sequence.setModifiers(e->modifiers());
    m_waitForPressTimer = startTimer(1500);
}

void FluxboxKeySequenceEdit::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    if(m_waitForPressTimer < 0)
        return;
    m_sequence.setMouseEventType(FluxboxMouseEventType::Click);
    m_sequence.setModifiers(e->modifiers());
    stopTimer(m_waitForPressTimer);
    emit editingFinished();
}

void FluxboxKeySequenceEdit::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    if(m_waitForPressTimer > -1)
    {
        m_sequence.setMouseEventType(FluxboxMouseEventType::Move);
        m_sequence.setModifiers(e->modifiers());
        stopTimer(m_waitForPressTimer);
        emit editingFinished();
    }
}

void FluxboxKeySequenceEdit::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == m_waitForPressTimer)
    {
        m_sequence.setMouseEventType(FluxboxMouseEventType::Press);
        m_waitForPressTimer = -1;
        emit editingFinished();
    }
}

bool FluxboxKeySequenceEdit::event(QEvent *e)
{
    switch(e->type())
    {
    case QEvent::Shortcut:
        return true;
    case QEvent::ShortcutOverride:
        return true;
    default:
        break;
    }
    return QLineEdit::event(e);
}

void FluxboxKeySequenceEdit::reset()
{
    m_sequence.unset();
    emit keySequenceChanged();
}

void FluxboxKeySequenceEdit::resetState()
{
    m_timeoutTimer = -1;
    m_waitForPressTimer = -1;
    m_sequence.unset();
}

void FluxboxKeySequenceEdit::updateDisplay()
{
    setText(m_sequence.toPrettyString());
}

void FluxboxKeySequenceEdit::stopTimer(int &id)
{
    killTimer(id);
    id = -1;
}

bool FluxboxKeySequenceEdit::isModifier(Qt::Key key)
{
    switch(key)
    {
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Alt:
    case Qt::Key_Meta:
        return true;
    default:
        return false;
    }
}

FluxboxKeySequenceEditWithMouseOptions::FluxboxKeySequenceEditWithMouseOptions(QWidget *parent)
    : QWidget(parent), m_editor(new FluxboxKeySequenceEdit), m_extraSettings(new QPushButton),
      m_reset(new QPushButton)
{
    setAutoFillBackground(true); // important so that item delegate painting isn't in the background
    m_extraSettings->setIcon(QIcon::fromTheme("configure"));
    m_reset->setIcon(QIcon::fromTheme("clear_left"));
    QHBoxLayout* lay = new QHBoxLayout;
    lay->setMargin(0);
    lay->setContentsMargins(1, 1, 1, 1);
    setLayout(lay);
    layout()->addWidget(m_editor);
    layout()->addWidget(m_reset);
    layout()->addWidget(m_extraSettings);
    connect(m_editor, &FluxboxKeySequenceEdit::keySequenceChanged, [this](){
        m_extraSettings->setDisabled(m_editor->keySequence().type() != FluxboxKeySequence::Type::Mouse);
    });
    connect(m_reset, &QPushButton::clicked, m_editor, &FluxboxKeySequenceEdit::reset);
    connect(m_extraSettings, &QPushButton::clicked, this, &FluxboxKeySequenceEditWithMouseOptions::launchExtraSettings);
    connect(m_editor, &FluxboxKeySequenceEdit::editingFinished, [this](){
        emit editingFinished();
    });
    connect(m_editor, &FluxboxKeySequenceEdit::keySequenceChanged, [this](){
        emit keySequenceChanged();
    });
}

void FluxboxKeySequenceEditWithMouseOptions::launchExtraSettings()
{
    QDialog dialog{this};
    dialog.setModal(true);
    dialog.setWindowTitle(NAME);
    dialog.setWindowIcon(ICON);
    dialog.setLayout(new QVBoxLayout);
    QDialogButtonBox* box = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};
    connect(box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    QMap<QString, FluxboxMouseModifier> mappings = {
        {"Double Click", FluxboxMouseModifier::Double},
        {"On Desktop", FluxboxMouseModifier::OnDesktop},
        {"On Window", FluxboxMouseModifier::OnWindow},
        {"On Toolbar", FluxboxMouseModifier::OnToolbar},
        {"On Titlebar", FluxboxMouseModifier::OnTitlebar},
        {"On Tab", FluxboxMouseModifier::OnTab},
        {"On Icon Button", FluxboxMouseModifier::OnIconButton},
        {"On Left Grip", FluxboxMouseModifier::OnLeftGrip},
        {"On Right Grip", FluxboxMouseModifier::OnRightGrip},
        {"On Window Border", FluxboxMouseModifier::OnWindowBorder},
        {"On Slit", FluxboxMouseModifier::OnSlit}
    };
    QVector<QCheckBox*> checkboxes;
    FluxboxMouseModifiers mods = m_editor->keySequence().mouseModifiers();
    for(auto key : mappings.keys())
    {
        QCheckBox* cb = new QCheckBox{key};
        cb->setChecked(mods & mappings[key]);
        checkboxes << cb;
    }
    for(auto cb : checkboxes)
    {
        dialog.layout()->addWidget(cb);
    }
    dialog.layout()->addWidget(box);
    int result = dialog.exec();
    if(result == QDialog::Accepted)
    {
        FluxboxMouseModifiers modifiers = m_editor->keySequence().mouseModifiers();
        for(auto cb : checkboxes)
        {
            modifiers.setFlag(mappings[cb->text()], cb->isChecked());
        }
        FluxboxKeySequence seq = m_editor->keySequence();
        seq.setMouseModifiers(modifiers);
        m_editor->setKeySequence(seq);
    }
}

class FluxboxKeySequenceEditV3 : public QLineEdit
{
    Q_OBJECT
public:
    FluxboxKeySequenceEditV3(QWidget* parent = nullptr);
    FluxboxKeySequenceEditV3(QList<FluxboxKeySequence> sequence, QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void timerEvent(QTimerEvent* e);
    bool event(QEvent* e);
    void setSequences(QList<FluxboxKeySequence> sequences);
    QList<FluxboxKeySequence> sequences() const;
public slots:
    void clear();
private:
    void init();
    void finishEditing();
    void updateDisplay();
    void resetState();
    bool m_recording;
    QList<FluxboxKeySequence> m_sequences;
    int releaseTimer = 0;
    int prevKey;
    const int MaxSequences = 4;
signals:
};

