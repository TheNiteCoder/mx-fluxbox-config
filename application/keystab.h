#ifndef KEYSTAB_H
#define KEYSTAB_H

#include "tab.h"

#include <QTableWidget>
#include <QKeySequenceEdit>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QSortFilterProxyModel>

#include "fluxboxkeysequence.h"
#include "fluxboxcommands.h"

namespace Ui
{
class KeysTab;
class KeySequencesEdit;
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
    void editingFinished();
};

class KeySequenceDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private slots:
    void commitEditorData();
};

class FluxboxCommandDelegate;

class FluxboxCommandDelegateUi : public QWidget
{
    Q_OBJECT
public:
    FluxboxCommandDelegateUi(AbstractFluxboxCommand* command, QWidget* parent = nullptr);
    void setCommand(AbstractFluxboxCommand* command) { m_command = command; m_configure->setDisabled(!m_command->configurable()); }
    AbstractFluxboxCommand* command() const { return m_command; }
public slots:
    void configure();
private:
    static AbstractFluxboxCommand* createCommandWithName(QString name);
    AbstractFluxboxCommand* m_command;
    QComboBox* m_commandTypeSelector;
    QPushButton* m_configure;
};

class FluxboxCommandDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    static const QList<QStringList> AvailableCommands;
    static const QStringList SingleAvailableCommands;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


enum UserTableWidgetItem
{
    KeyboardSequence = QTableWidgetItem::UserType,
};

using QKeySequenceList = QList<QKeySequence>;

enum UserRole
{
    DataRole = Qt::UserRole,
};

class DialogDelegate : public QObject
{
    Q_OBJECT
public:
    DialogDelegate(QObject* parent = nullptr);
    virtual QWidget* createEditor(QWidget* parent) const { return new QWidget{parent}; }
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const { Q_UNUSED(editor) Q_UNUSED(index) }
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const { Q_UNUSED(editor) Q_UNUSED(model) Q_UNUSED(index) }
    virtual bool checkValidData(const QModelIndex&) const { return true; }
};

class KeySequencesDelegateUi : public QWidget
{
    Q_OBJECT
public:
    KeySequencesDelegateUi(QWidget* parent = nullptr);
    ~KeySequencesDelegateUi();
    Ui::KeySequencesEdit* ui;
    QStandardItemModel model;
    FluxboxKeySequenceList keySequences() const;
    void setKeySequences(const FluxboxKeySequenceList& list);
    KeySequenceDelegate delegate;
};

class KeySequencesDialogDelegate : public DialogDelegate
{
    Q_OBJECT
public:
    QWidget* createEditor(QWidget* parent) const override { return new KeySequencesDelegateUi{parent}; }
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

class CommandDialogDelegateUi : public QWidget
{
    Q_OBJECT
public:
    CommandDialogDelegateUi(QWidget* parent = nullptr);
    void setCommand(AbstractFluxboxCommand* command);
    AbstractFluxboxCommand* command() const { return m_command; }
    void setKeySequences(FluxboxKeySequenceList sequences) { m_sequences = sequences; onKeySequencesChanged(); }
private slots:
    void newCommandOn();
    void newCommandOff();
    void onTextChanged();
private:
    void onKeySequencesChanged();
    QComboBox* m_commandTypeSelector;
    QWidget* m_editor;
    AbstractFluxboxCommand* m_command;
    FluxboxKeySequenceList m_sequences;
    friend class CommandDialogDelegate;
};

class CommandDialogDelegate : public DialogDelegate
{
    Q_OBJECT
public:
    QWidget* createEditor(QWidget* parent) const override { return new CommandDialogDelegateUi{parent}; }
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

class KeysTab : public Tab
{
    Q_OBJECT
public:
    explicit KeysTab(QObject *parent = nullptr);
    void setup(FluxboxSource source);
    void apply(FluxboxSource& source);
    struct Mapping
    {
        QList<FluxboxKeySequence> sequences;
        QString command;
    };
    bool readMapping(QString line, Mapping& mapping);
    void removeComment(QString& line);
    static QString replaceKeyNames(QString str, bool reverse = false);
    static QString resolveKeysequences(QString str);
    static QList<QKeySequence> keySequences(QString string);
    static QString keyString(QList<QKeySequence> sequences);
    static QString addNones(QString string);
    static QString compareKeyStrings(QString left, QString right);
    static QKeySequence joinKeySequences(QKeySequenceList sequences);
    void addRow(QKeySequence sequence, QString command);
    static void setIndex(QAbstractItemModel* model, const QModelIndex& index, QVariant value);
public slots:
    void openEditor(const QModelIndex& index);
private:
    void updateDisplay(const QModelIndex& index);
    QMap<int, DialogDelegate*> m_delegates;
    QList<Mapping> m_mappings;
    friend QDebug operator<<(QDebug debug, const Mapping& mapping);
    friend class KeyboardShortcutItem;
    QStandardItemModel m_model;
    Ui::KeysTab* ui;
    KeySequenceDelegate keySequenceDelegate;
    FluxboxCommandDelegate commandDelegate;
    KeySequencesDialogDelegate keySequencesDialogDelegate;
    CommandDialogDelegate commandDialogDelegate;
    QModelIndex m_editorIndex;
};

#endif // KEYSTAB_H
