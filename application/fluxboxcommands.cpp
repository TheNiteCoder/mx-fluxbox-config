
#include "fluxboxcommands.h"

#include <QHBoxLayout>

#define M(cmd) {cmd, true}
#define C(cmd) {cmd, false}
const QMap<QString, bool> FluxboxCommandInfo::AvailableCommands = {
    C("Unknown"),
    M("StartMoving"),
    M("StartResizing"),
    M("StartTabbing"),
    M("ActivateTab"),
    C("Unknown"),
    C("Exec"),
    C("Workspace"),
    C("Tab"),
    C("Close"),
    C("Minimize"),
    C("Maximize"),
    C("MaximizeHorizontal"),
    C("MaximizeVertical"),
    C("Fullscreen"),
    C("Raise"),
    C("Lower"),
    C("Kill"),
    C("Shade"),
    C("ShadeOn"),
    C("ShadeOff"),
    C("Stick"),
    C("ToggleDecor"),
    C("NextTab"),
    C("PrevTab"),
    C("MoveTabLeft"),
    C("MoveTabRight"),
    C("DetachClient"),
    C("AddWorkspace"),
    C("RemoveLastWorkspace"),
    C("FocusLeft"),
    C("FocusRight"),
    C("FocusUp"),
    C("FocusDown"),
    C("ShowDesktop"),
    C("CloseAllWindows"),
    C("SendToWorkspace"),
    C("TakeToWorkspace"),
    C("RootMenu"),
    C("WindowMenu"),
    C("HideMenus"),
    C("WorkspaceMenu"),
};
#undef M
#undef C


AbstractFluxboxCommand::~AbstractFluxboxCommand()
{
}

QStringList AbstractFluxboxCommand::split(QString input)
{
    QStringList tokens;
    while(true)
    {
        QString token;
        input = input.trimmed();
        if(input.size() == 0) break;
        if(input[0] == '{')
        {
            int ending = input.indexOf('}');
            if(ending == -1) // if no ending } take the rest of string and make it a token and stop parsing
            {
                tokens << input;
                break;
            }
            tokens << input.mid(1, ending - 1);
            input.remove(0, ending + 1);
        }
        else
        {
            int ending = -1;
            if(input.contains('{') && input.contains(' '))
                ending = qMin(input.indexOf('{'), input.indexOf(' '));
            else if(input.contains(' '))
                ending = input.indexOf(' ');
            else if(input.contains('{'))
                ending = input.indexOf('{');
            if(ending == -1) // if no ending space take the rest of string and make it a token and stop parsing
            {
                tokens << input;
                break;
            }
            tokens << input.mid(0, ending);
            input.remove(0, ending);
        }
    }
    return tokens;
}

QString AbstractFluxboxCommand::retrieveCommandName(QString &input)
{
    input = input.trimmed();
    int ending = input.indexOf(' ');
    if(ending == -1)
    {
        QString saved = input;
        input = "";
        return saved;
    }
    QString name = input.mid(0, ending);
    input.remove(0, ending + 1);
    input = input.trimmed();
    return name;
}

void ExecFluxboxCommand::construct()
{
    m_cmd = arg();
}

void ExecFluxboxCommand::setEditorData(QWidget *editor, const AbstractFluxboxCommand *command)
{
    ExecFluxboxCommandUi* ui = dynamic_cast<ExecFluxboxCommandUi*>(editor);
    Q_ASSERT(ui != nullptr);
    const ExecFluxboxCommand* cmd = dynamic_cast<const ExecFluxboxCommand*>(command);
    Q_ASSERT(cmd != nullptr);
    ui->commandEditor->setText(cmd->m_cmd);
}

void ExecFluxboxCommand::setCommandData(QWidget *editor, AbstractFluxboxCommand *command)
{
    ExecFluxboxCommandUi* ui = dynamic_cast<ExecFluxboxCommandUi*>(editor);
    Q_ASSERT(ui != nullptr);
    ExecFluxboxCommand* cmd = dynamic_cast<ExecFluxboxCommand*>(command);
    Q_ASSERT(cmd != nullptr);
    cmd->m_cmd = ui->commandEditor->text();
}

ExecFluxboxCommandUi::ExecFluxboxCommandUi(QWidget *parent)
    : QWidget(parent), commandEditor(new QLineEdit)
{
    setLayout(new QHBoxLayout);
    layout()->addWidget(commandEditor);
}

AbstractFluxboxCommand *FluxboxCommandDispatcher::dispatch(QString input)
{
    QString full = input;
    QString name = AbstractFluxboxCommand::retrieveCommandName(input);
    AbstractFluxboxCommand* result = nullptr;
    if(name.indexOf("exec", 0, Qt::CaseInsensitive) == 0)
    {
        result = new ExecFluxboxCommand;
    }
#define CASE(str, type) else if(name.indexOf(str, 0, Qt::CaseInsensitive) == 0) { result = new type; }
    CASE("RootMenu", RootMenuFluxboxCommand)
    CASE("WindowMenu", WindowMenuFluxboxCommand)
    CASE("HideMenus", HideMenusFluxboxCommand)
    CASE("WorkspaceMenu", WorkspaceMenuFluxboxCommand)
    CASE("Workspace", WorkspaceFluxboxCommand)
    CASE("Tab", TabFluxboxCommand)
    CASE("Close", CloseFluxboxCommand)
    CASE("Minimize", MinimizeFluxboxCommand)
    CASE("Maximize", MaximizeFluxboxCommand)
    CASE("MaximizeHorizontal", MaximizeHorizontalFluxboxCommand)
    CASE("MaximizeVertical", MaximizeVerticalFluxboxCommand)
    CASE("Fullscreen", FullscreenFluxboxCommand)
    CASE("Raise", RaiseFluxboxCommand)
    CASE("Lower", LowerFluxboxCommand)
    CASE("StartMoving", StartMovingFluxboxCommand)
    CASE("ActivateTab", ActivateTabFluxboxCommand)
    CASE("StartTabbing", StartTabbingFluxboxCommand)
    CASE("Kill", KillFluxboxCommand)
    CASE("Shade", ShadeFluxboxCommand)
    CASE("ShadeOn", ShadeOnFluxboxCommand)
    CASE("ShadeOff", ShadeOffFluxboxCommand)
    CASE("Stick", StickFluxboxCommand)
    CASE("ToggleDecor", ToggleDecorFluxboxCommand)
    CASE("NextTab", NextTabFluxboxCommand)
    CASE("PrevTab", PrevTabFluxboxCommand)
    CASE("MoveTabLeft", MoveTabLeftFluxboxCommand)
    CASE("MoveTabRight", MoveTabRightFluxboxCommand)
    CASE("DetachClient", DetachClientFluxboxCommand)
    CASE("AddWorkspace", AddWorkspaceFluxboxCommand)
    CASE("RemoveLastWorkspace", RemoveLastWorkspaceFluxboxCommand)
    CASE("FocusLeft", FocusLeftFluxboxCommand)
    CASE("FocusRight", FocusRightFluxboxCommand)
    CASE("FocusUp", FocusUpFluxboxCommand)
    CASE("FocusDown", FocusDownFluxboxCommand)
    CASE("ShowDesktop", ShowDesktopFluxboxCommand)
    CASE("CloseAllWindows", CloseAllWindowsFluxboxCommand)
    CASE("SendToWorkspace", SendToWorkspaceCommand)
    CASE("TakeToWorkspace", TakeToWorkspaceCommand)
#undef CASE
    // DEBUG << "input: " << input << ", name: " << name << ", result" << result;
    if(result == nullptr)
    {
        result = new UnknownFluxboxCommand;
        result->init("Unknown", full);
    }
    else
    {
        result->init(name, input);
    }
    return result;
}

