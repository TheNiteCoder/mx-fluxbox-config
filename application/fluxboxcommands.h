#ifndef FLUXBOXCOMMANDS_H
#define FLUXBOXCOMMANDS_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>

struct FluxboxCommandInfo
{
    static const QMap<QString, bool> AvailableCommands;
};

class AbstractFluxboxCommand : public QObject
{
    Q_OBJECT
public:
    virtual ~AbstractFluxboxCommand();
    void init(QString name, QString args) { m_args = args; m_name = name; construct(); }
    static QStringList split(QString input);
    static QString retrieveCommandName(QString& input);
    virtual QWidget* createEditor(QWidget* parent) { Q_UNUSED(parent) return nullptr; }
    virtual void destoryEditor(QWidget* editor) { editor->deleteLater(); }
    virtual void setEditorData(QWidget* editor, const AbstractFluxboxCommand* command) { Q_UNUSED(editor) Q_UNUSED(command) }
    virtual void setCommandData(QWidget* editor, AbstractFluxboxCommand* command) { Q_UNUSED(editor) Q_UNUSED(command) }
    QString arg() const { return m_args; }
    QString name() const { return m_name; }
    virtual QString toString() const = 0;
    virtual bool configurable() { return true; }
    bool equal(AbstractFluxboxCommand* other) { Q_ASSUME(other != nullptr); return toString() == other->toString(); }
protected:
    virtual void construct() = 0;
    QString m_args;
    QString m_name;
};

class UnknownFluxboxCommandUi : public QWidget
{
    Q_OBJECT
public:
    UnknownFluxboxCommandUi(QWidget* parent = nullptr);
    QLineEdit* editor;
};

class UnknownFluxboxCommand : public AbstractFluxboxCommand
{
    Q_OBJECT
public:
    void construct();
    QWidget* createEditor(QWidget* parent) { return new UnknownFluxboxCommandUi{parent}; }
    void setEditorData(QWidget* editor, const AbstractFluxboxCommand* command);
    void setCommandData(QWidget* editor, AbstractFluxboxCommand* command);
    QString toString() const { return m_cmd; }
private:
    QString m_cmd;
};

class ExecFluxboxCommandUi : public QWidget
{
    Q_OBJECT
public:
    ExecFluxboxCommandUi(QWidget* parent = nullptr);
    QLineEdit* commandEditor;
};

class ExecFluxboxCommand : public AbstractFluxboxCommand
{
    Q_OBJECT
public:
    void construct();
    QWidget* createEditor(QWidget* parent) { return new ExecFluxboxCommandUi{parent}; }
    void setEditorData(QWidget* editor, const AbstractFluxboxCommand* command);
    void setCommandData(QWidget* editor, AbstractFluxboxCommand* command);
    QString toString() const { return QString("Exec %1").arg(m_cmd); }
private:
    QString m_cmd;
};

#define FC_UI(type) type* ui = dynamic_cast<type*>(editor)
#define FC_CMD(type) type* cmd = dynamic_cast<type*>(command)

class WorkspaceFluxboxCommandUi : public QWidget
{
    Q_OBJECT
public:
    WorkspaceFluxboxCommandUi(QWidget* parent = nullptr);
    QSpinBox* workspaceNumberEditor;
};

class WorkspaceFluxboxCommand : public AbstractFluxboxCommand
{
    Q_OBJECT
public:
    void construct();
    QWidget* createEditor(QWidget* parent) { return new WorkspaceFluxboxCommandUi{parent}; }
    void setEditorData(QWidget* editor, const AbstractFluxboxCommand* command);
    void setCommandData(QWidget* editor, AbstractFluxboxCommand* command);
    QString toString() const { return QString("Workspace %1").arg(m_workspaceNumber); }
protected:
    int m_workspaceNumber;
};

class TakeToWorkspaceCommand : public WorkspaceFluxboxCommand
{
    Q_OBJECT
public:
    QString toString() const { return QString("TakeToWorkspace %1").arg(m_workspaceNumber); }
};

class SendToWorkspaceCommand : public WorkspaceFluxboxCommand
{
    Q_OBJECT
public:
    QString toString() const { return QString("SendToWorkspace %1").arg(m_workspaceNumber); }
};

class TabFluxboxCommandUi : public QWidget
{
    Q_OBJECT
public:
    TabFluxboxCommandUi(QWidget* parent = nullptr);
    QSpinBox* tabNumberEditor;
};

class TabFluxboxCommand : public AbstractFluxboxCommand
{
    Q_OBJECT
public:
    void construct();
    QWidget* createEditor(QWidget* parent) { return new TabFluxboxCommandUi{parent}; }
    void setEditorData(QWidget* editor, const AbstractFluxboxCommand* command);
    void setCommandData(QWidget* editor, AbstractFluxboxCommand* command);
    QString toString() const { return QString("Tab %1").arg(m_tabNumber); }
private:
    int m_tabNumber;
};

#define NO_ARG_COMMAND(name) class name##FluxboxCommand : public AbstractFluxboxCommand {\
    Q_OBJECT public:\
    void construct() {}\
    QWidget* createEditor(QWidget* parent) { return new QLabel("No Settings", parent); }\
    void setEditorData(QWidget*, const AbstractFluxboxCommand*) {}\
    void setCommandData(QWidget*, AbstractFluxboxCommand*) {}\
    QString toString() const { return QString(#name); }\
    bool configurable() { return false; }\
    };

NO_ARG_COMMAND(Close)
NO_ARG_COMMAND(Minimize)
NO_ARG_COMMAND(Maximize)
NO_ARG_COMMAND(MaximizeHorizontal)
NO_ARG_COMMAND(MaximizeVertical)
NO_ARG_COMMAND(Fullscreen)
NO_ARG_COMMAND(Raise)
NO_ARG_COMMAND(Lower)
NO_ARG_COMMAND(StartMoving)
NO_ARG_COMMAND(ActivateTab)
NO_ARG_COMMAND(StartTabbing)
NO_ARG_COMMAND(Kill)
NO_ARG_COMMAND(Shade)
NO_ARG_COMMAND(ShadeOn)
NO_ARG_COMMAND(ShadeOff)
NO_ARG_COMMAND(Stick)
NO_ARG_COMMAND(ToggleDecor)
NO_ARG_COMMAND(NextTab)
NO_ARG_COMMAND(PrevTab)
NO_ARG_COMMAND(MoveTabLeft)
NO_ARG_COMMAND(MoveTabRight)
NO_ARG_COMMAND(DetachClient)
NO_ARG_COMMAND(AddWorkspace)
NO_ARG_COMMAND(RemoveLastWorkspace)
NO_ARG_COMMAND(FocusLeft)
NO_ARG_COMMAND(FocusRight)
NO_ARG_COMMAND(FocusUp)
NO_ARG_COMMAND(FocusDown)
NO_ARG_COMMAND(ShowDesktop)
NO_ARG_COMMAND(CloseAllWindows)
NO_ARG_COMMAND(RootMenu)
NO_ARG_COMMAND(WorkspaceMenu)
NO_ARG_COMMAND(WindowMenu)
NO_ARG_COMMAND(HideMenus)

#undef NO_ARG_COMMAND

struct FluxboxCommandDispatcher
{
    static AbstractFluxboxCommand* dispatch(QString input);
};


#endif // FLUXBOXCOMMANDS_H
