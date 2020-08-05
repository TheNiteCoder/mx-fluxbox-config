#include "keystab.h"

#include "ui_keystab.h"
#include "ui_keysequencesedit.h"

#include <QPainter>
#include <QKeySequenceEdit>
#include <QDialog>
#include <QDialogButtonBox>

#include "global.h"
#include "fluxboxkeysequenceedit.h"
#include "fluxboxkeysstream.h"

const QList<QStringList> FluxboxCommandDelegate::AvailableCommands = {
    {"Exec"}
};

const QStringList FluxboxCommandDelegate::SingleAvailableCommands = {
};

KeysTab::KeysTab(QObject *parent)
    : Tab("Keys", QIcon::fromTheme("keyboard"), parent), ui(new Ui::KeysTab)
{
    setWidget(new QWidget);
    ui->setupUi(widget());
    ui->tableView_Keys->setModel(&m_model);
    //ui->tableView_Keys->setItemDelegateForColumn(0, &keySequenceDelegate);
    //ui->tableView_Keys->setItemDelegateForColumn(1, &commandDelegate);
    ui->tableView_Keys->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_Keys->verticalHeader()->setVisible(false);
    ui->tableView_Keys->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_Keys->setCornerButtonEnabled(false);
    ui->tableView_Keys->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_Keys->setSelectionBehavior(QTableView::SelectRows);
    //ui->tableView_Keys->sortByColumn(1, Qt::AscendingOrder);
    m_model.setHorizontalHeaderLabels({tr("Shortcut"), tr("Command")});
    m_delegates[0] = &keySequencesDialogDelegate;
    m_delegates[1] = &commandDialogDelegate;
    connect(ui->tableView_Keys, &QTableView::doubleClicked, [this](const QModelIndex& index){
        openEditor(index);
    });
    connect(ui->pushButton_Add, &QPushButton::clicked, [this](){
        QStandardItem* item0 = new QStandardItem;
        QVariant value;
        value.setValue(FluxboxKeySequenceList());
        item0->setData(value, UserRole::DataRole);
        item0->setData("", Qt::DisplayRole);
        AbstractFluxboxCommand* command = new UnknownFluxboxCommand;
        command->init("", "");
        QStandardItem* item1 = new QStandardItem;
        QVariant value1;
        value1.setValue(command);
        item1->setData(value1, UserRole::DataRole);
        item1->setData("", Qt::DisplayRole);
        m_model.appendRow({item0, item1});
        ui->tableView_Keys->scrollToBottom();
    });
    connect(ui->pushButton_Remove, &QPushButton::clicked, [this](){
        QList<int> rows;
        for(QModelIndex index : ui->tableView_Keys->selectionModel()->selectedRows())
        {
            rows << index.row();
        }
        std::sort(rows.begin(), rows.end(), std::greater<int>());
        for(int row : rows)
        {
            m_model.removeRow(row);
        }
    });
}

void KeysTab::setup(FluxboxSource source)
{
    QString keys = source["keys"];
    FluxboxKeysStream stream{&keys};
    for(FluxboxKeysStream::Mapping mapping : stream.parseMappings())
    {
        QStandardItem* item0 = new QStandardItem;
        QVariant variant0;
        variant0.setValue(mapping.sequences);
        item0->setData(variant0, UserRole::DataRole);
        item0->setData(FluxboxKeySequence::listToPrettyString(mapping.sequences), Qt::DisplayRole);
        QStandardItem* item1 = new QStandardItem;
        QVariant value;
        value.setValue(mapping.command);
        item1->setData(value, UserRole::DataRole);
        item1->setData(mapping.command->toString(), Qt::DisplayRole);
        m_model.appendRow({item0, item1});
    }
}

void KeysTab::apply(FluxboxSource& source)
{
    QString keys = source["keys"];
    FluxboxKeysStream stream{&keys};
    FluxboxKeysStream::MappingList list;
    for(int i = 0; i < m_model.rowCount(); i++)
    {
        QStandardItem* item0 = m_model.item(i, 0);
        QStandardItem* item1 = m_model.item(i, 1);
        FluxboxKeysStream::Mapping mapping;
        mapping.sequences = item0->data(UserRole::DataRole).value<FluxboxKeySequenceList>();
        mapping.command = item1->data(UserRole::DataRole).value<AbstractFluxboxCommand*>();
        if(mapping.command->toString() == "")
            continue;
        if(mapping.sequences.size() < 1)
            continue;
        list << mapping;
    }
    stream.setMappings(list);
    source["keys"] = keys;
}

bool KeysTab::readMapping(QString line, KeysTab::Mapping &mapping)
{
    removeComment(line);
    QStringList keysAndCommand = line.split(':', QString::SkipEmptyParts);
    if(keysAndCommand.size() < 2) return false;
    QString keys = keysAndCommand.at(0);
    QString command = keysAndCommand.at(1);
    mapping.sequences = FluxboxKeySequence::listFromString(keys);
    mapping.command = command;
    return true;
}

void KeysTab::removeComment(QString &line)
{
    int pos = (line.contains('#') ? line.indexOf('#') : line.indexOf('!'));
    if(pos == -1) return;
    line = line.mid(0, pos);
}

QString KeysTab::replaceKeyNames(QString str, bool reverse)
{
    auto iter = keyReplacement.constBegin();
    while(iter != keyReplacement.constEnd())
    {
        if(reverse)
            str.replace(iter.value(), iter.key());
        else
            str.replace(iter.key(), iter.value());
        ++iter;
    }
    return str;
}

QString KeysTab::resolveKeysequences(QString str)
{
    QStringList parts = str.split(' ', QString::SkipEmptyParts);
    std::for_each(parts.begin(), parts.end(), [](QString& s){
        s = s.trimmed();
    });
    QString result;
    for(QString part : parts)
    {
        if(modifierKeys.contains(part, Qt::CaseInsensitive))
        {
            result.append(part);
            result.append('+');
        }
        else
        {
            result.append(part);
            result.append(',');
        }
    }
    if(result.endsWith(','))
    {
        result.chop(1);
    }
    return result;
}

QList<QKeySequence> KeysTab::keySequences(QString string)
{
    string.remove("none", Qt::CaseInsensitive);
    string = replaceKeyNames(string);
    string = resolveKeysequences(string);
    return QKeySequence::listFromString(string, QKeySequence::NativeText);
}

QString KeysTab::keyString(QList<QKeySequence> sequences)
{
    QStringList result;
    for(QKeySequence seq : sequences)
    {
        QString native = seq.toString(QKeySequence::NativeText);
        native = replaceKeyNames(native, true);
        native.replace('+', " ");
        native = addNones(native);
        result << native;
    }
    return result.join(' ');
}

QString KeysTab::addNones(QString string)
{
    QStringList keys = string.split(' ', QString::SkipEmptyParts);
    std::for_each(keys.begin(), keys.end(), [](QString& s){
        s = s.trimmed();
    });
    if(keys.size() < 2)
    {
        keys.push_front("none");
    }
    return keys.join(' ');
}

QKeySequence KeysTab::joinKeySequences(QKeySequenceList sequences)
{
    return QKeySequence::fromString(QKeySequence::listToString(sequences));
}

void KeysTab::openEditor(const QModelIndex &index)
{
    if(!m_delegates.contains(index.column()))
    {
        DEBUG << "No delegate for column" << index.column();
        return;
    }
    QDialog dialog{widget()};
    dialog.setWindowTitle(NAME);
    dialog.setWindowIcon(ICON);
    QDialogButtonBox box{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};
    connect(&box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    dialog.setLayout(new QVBoxLayout);
    DialogDelegate* delegate = m_delegates[index.column()];
    QWidget* editor = delegate->createEditor(&dialog);
    delegate->setEditorData(editor, index);
    dialog.layout()->addWidget(editor);
    dialog.layout()->addWidget(&box);
    int result = dialog.exec();
    if(result == QDialog::Accepted)
    {
        delegate->setModelData(editor, &m_model, index);
    }
}

void KeysTab::setIndex(QAbstractItemModel *model, const QModelIndex &index, QVariant value)
{
    model->setData(index, value, UserRole::DataRole);
    if(value.canConvert<FluxboxKeySequence>())
        model->setData(index, value.value<FluxboxKeySequence>().toPrettyString(), Qt::DisplayRole);
    else if(value.canConvert<AbstractFluxboxCommand*>())
        model->setData(index, value.value<AbstractFluxboxCommand*>()->toString(), Qt::DisplayRole);
}

void KeysTab::updateDisplay(const QModelIndex &index)
{
    if(m_model.data(index, UserRole::DataRole).canConvert<QList<FluxboxKeySequence>>())
    {
        m_model.setData(index, FluxboxKeySequence::listToString(
                            m_model.data(index, UserRole::DataRole).value<QList<FluxboxKeySequence>>()), Qt::DisplayRole);
    }
    else if(m_model.data(index, UserRole::DataRole).canConvert<AbstractFluxboxCommand*>())
    {
        m_model.setData(index, m_model.data(index, UserRole::DataRole).value<AbstractFluxboxCommand*>()->toString(),
                        Qt::DisplayRole);
    }
}

QWidget *KeySequenceDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    FluxboxKeySequenceEditWithMouseOptions* editor = new FluxboxKeySequenceEditWithMouseOptions{parent};
    connect(editor, &FluxboxKeySequenceEditWithMouseOptions::keySequenceChanged, this, &KeySequenceDelegate::commitEditorData);
    return editor;
}

void KeySequenceDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.model()->data(index, UserRole::DataRole).canConvert<FluxboxKeySequence>())
    {
        FluxboxKeySequence sequence = index.model()->data(index, UserRole::DataRole).value<FluxboxKeySequence>();
        FluxboxKeySequenceEditWithMouseOptions* edit = dynamic_cast<FluxboxKeySequenceEditWithMouseOptions*>(editor);
        edit->setKeySequence(sequence);
    }
    else
    {
        DEBUG << "Big problems";
    }
}

void KeySequenceDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    FluxboxKeySequenceEditWithMouseOptions* edit = dynamic_cast<FluxboxKeySequenceEditWithMouseOptions*>(editor);
    Q_ASSERT(edit != nullptr);
    FluxboxKeySequence sequence = edit->keySequence();
    QVariant variant;
    variant.setValue(sequence);
    model->setData(index, variant, UserRole::DataRole);
    model->setData(index, sequence.toPrettyString(), Qt::DisplayRole);
    //KeysTab::setIndex(model, index, variant);
}

void KeySequenceDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void KeySequenceDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.data(UserRole::DataRole).canConvert<FluxboxKeySequence>())
    {
        painter->save();
        if(option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());
        painter->drawText(option.rect, index.data(UserRole::DataRole).value<FluxboxKeySequence>().toPrettyString(),
        {Qt::AlignLeft | Qt::AlignCenter});
        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize KeySequenceDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.model()->data(index, UserRole::DataRole).canConvert<FluxboxKeySequence>())
    {
        FluxboxKeySequence seq = index.model()->data(index, UserRole::DataRole).value<FluxboxKeySequence>();
        QSize size = option.fontMetrics.size(0, seq.toPrettyString());
        return QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem, &option, size);
    }
    return QSize();
}

void KeySequenceDelegate::commitEditorData()
{
    emit commitData(dynamic_cast<QWidget*>(sender()));
}


FluxboxKeySequenceEditV3::FluxboxKeySequenceEditV3(QWidget *parent)
    : QLineEdit(parent)
{
    init();
}

FluxboxKeySequenceEditV3::FluxboxKeySequenceEditV3(QList<FluxboxKeySequence> sequence, QWidget *parent)
    : QLineEdit(parent), m_sequences(sequence)
{
    init();
}

void FluxboxKeySequenceEditV3::keyPressEvent(QKeyEvent *e)
{
    int nextKey = e->key();

    if(prevKey == -1)
    {
        clear();
        prevKey = nextKey;
    }

    switch(nextKey)
    {
    case Qt::Key_Control: return;
    case Qt::Key_Shift: return;
    case Qt::Key_Alt: return;
    case Qt::Key_Meta: return;
    case Qt::Key_unknown: return;
    }

    FluxboxKeySequence seq;
    seq.setModifiers(e->modifiers());
    seq.setKey(static_cast<Qt::Key>(e->key()));
    m_sequences << seq;
    updateDisplay();
    e->accept();
}

void FluxboxKeySequenceEditV3::keyReleaseEvent(QKeyEvent *e)
{
    if(m_sequences.size() < MaxSequences)
        releaseTimer = startTimer(1000);
    else
        finishEditing();
    e->accept();
}

void FluxboxKeySequenceEditV3::mousePressEvent(QMouseEvent *e)
{
    if(releaseTimer == 0) clear();
    FluxboxKeySequence seq;
    seq.setModifiers(e->modifiers());
    seq.setMouseButton(FluxboxKeySequence::toFluxboxMouseButton(e->button()));
    m_sequences << seq;
    updateDisplay();
    e->accept();
}

void FluxboxKeySequenceEditV3::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_sequences.size() < MaxSequences)
        releaseTimer = startTimer(1000);
    else
        finishEditing();
    e->accept();
}

void FluxboxKeySequenceEditV3::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == releaseTimer)
    {
        finishEditing();
        return;
    }
    QLineEdit::timerEvent(e);
}

bool FluxboxKeySequenceEditV3::event(QEvent *e)
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

void FluxboxKeySequenceEditV3::setSequences(QList<FluxboxKeySequence> sequences)
{
    m_sequences = sequences;
}

QList<FluxboxKeySequence> FluxboxKeySequenceEditV3::sequences() const
{
    return m_sequences;
}

void FluxboxKeySequenceEditV3::clear()
{
    m_sequences.clear();
    updateDisplay();
}

void FluxboxKeySequenceEditV3::init()
{
    resetState();
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_MacShowFocusRect, true);
    setAttribute(Qt::WA_InputMethodEnabled, false);
    setPlaceholderText("Press to begin shortcuts");
}

void FluxboxKeySequenceEditV3::finishEditing()
{
    updateDisplay();
    resetState();
    emit editingFinished();
}

void FluxboxKeySequenceEditV3::updateDisplay()
{
    QStringList list;
    for(FluxboxKeySequence seq : m_sequences) list << seq.toString();
    QString text;
    if(m_recording && list.size() > 0)
        text = QString("%1, ...").arg(list.join(", "));
    else
        text = QString("%1").arg(list.join(", "));
    setText(text);
}

void FluxboxKeySequenceEditV3::resetState()
{
    prevKey = -1;
    if(releaseTimer)
    {
        killTimer(releaseTimer);
        releaseTimer = 0;
    }
}

QWidget *FluxboxCommandDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    if(index.model()->data(index, UserRole::DataRole).canConvert<AbstractFluxboxCommand*>())
    {
        AbstractFluxboxCommand* command = index.model()->data(index, UserRole::DataRole).value<AbstractFluxboxCommand*>();
        return new FluxboxCommandDelegateUi{command, parent};
    }
    return nullptr;
}

void FluxboxCommandDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.model()->data(index, UserRole::DataRole).canConvert<AbstractFluxboxCommand*>())
    {
        AbstractFluxboxCommand* command = index.model()->data(index, UserRole::DataRole).value<AbstractFluxboxCommand*>();
        FluxboxCommandDelegateUi* ui = dynamic_cast<FluxboxCommandDelegateUi*>(editor);
        ui->setCommand(command);
    }
}

void FluxboxCommandDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.model()->data(index, UserRole::DataRole).canConvert<AbstractFluxboxCommand*>())
    {
        FluxboxCommandDelegateUi* ui = dynamic_cast<FluxboxCommandDelegateUi*>(editor);
        QVariant value;
        value.setValue(ui->command());
        model->setData(index, value, UserRole::DataRole);
        model->setData(index, ui->command()->toString(), Qt::DisplayRole);
        //KeysTab::setIndex(model, index, value);
    }
}

void FluxboxCommandDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    FluxboxCommandDelegateUi* ui = static_cast<FluxboxCommandDelegateUi*>(editor);
    ui->setGeometry(option.rect);
}

void FluxboxCommandDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    if(option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    else // override previously drawn text
        painter->fillRect(option.rect, option.palette.base());
    if(index.model()->data(index, UserRole::DataRole).canConvert<AbstractFluxboxCommand*>() &&
            !option.state.testFlag(QStyle::State_HasFocus))
    {
        AbstractFluxboxCommand* cmd = index.model()->data(index, UserRole::DataRole).value<AbstractFluxboxCommand*>();
        painter->drawText(option.rect, cmd->toString(), {Qt::AlignCenter | Qt::AlignVCenter});
    }
    painter->restore();
}

QSize FluxboxCommandDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.model()->data(index, UserRole::DataRole).canConvert<AbstractFluxboxCommand*>())
    {
        AbstractFluxboxCommand* command = index.model()->data(index, UserRole::DataRole).value<AbstractFluxboxCommand*>();
        QSize size = option.fontMetrics.size(0, command->toString());
        return QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem, &option, size);
    }
    return QSize();
}

FluxboxCommandDelegateUi::FluxboxCommandDelegateUi(AbstractFluxboxCommand *command, QWidget *parent)
    : QWidget(parent), m_command(command), m_commandTypeSelector(new QComboBox),
      m_configure(new QPushButton)
{
    QHBoxLayout* lay = new QHBoxLayout;
    lay->setMargin(0);
    lay->setContentsMargins(0, 0, 0, 0);
    setLayout(lay);
    m_commandTypeSelector->addItems(FluxboxCommandDelegate::SingleAvailableCommands);
    m_commandTypeSelector->setCurrentText(m_command->name());
    connect(m_commandTypeSelector, &QComboBox::currentTextChanged, [this](){
        m_command = createCommandWithName(m_commandTypeSelector->currentText());
        Q_ASSERT(m_command != nullptr);
    });
    m_configure->setIcon(QIcon::fromTheme("configure"));
    m_configure->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_configure, &QPushButton::clicked, this, &FluxboxCommandDelegateUi::configure);
    m_configure->setDisabled(!m_command->configurable());
    layout()->addWidget(m_commandTypeSelector);
    layout()->addWidget(m_configure);
}

void FluxboxCommandDelegateUi::configure()
{
    QDialog d{this};
    d.setWindowTitle(NAME);
    d.setWindowIcon(ICON);
    d.setLayout(new QVBoxLayout);
    QDialogButtonBox box{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};
    connect(&box, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(&box, &QDialogButtonBox::rejected, &d, &QDialog::reject);
    QWidget* ui = m_command->createEditor(&d);
    m_command->setEditorData(ui, m_command); // populate UI with command object data
    d.layout()->addWidget(ui);
    d.layout()->addWidget(&box);
    int result = d.exec();

    // if accepted update command object with input from the UI
    if(result == QDialog::Accepted)
    {
        m_command->setCommandData(ui, m_command);
    }
}

AbstractFluxboxCommand* FluxboxCommandDelegateUi::createCommandWithName(QString name)
{
    AbstractFluxboxCommand* command = FluxboxCommandDispatcher::dispatch(name);
    return command;
}

WorkspaceFluxboxCommandUi::WorkspaceFluxboxCommandUi(QWidget *parent)
    : QWidget(parent), workspaceNumberEditor(new QSpinBox)
{
    setLayout(new QHBoxLayout);
    workspaceNumberEditor->setRange(1, 20);
    layout()->addWidget(workspaceNumberEditor);
}

void WorkspaceFluxboxCommand::construct()
{
    QString input = arg().trimmed();
    bool sucess = false;
    int i = input.toInt(&sucess);
    if(!sucess) i = 1;
    m_workspaceNumber = i;
}

void WorkspaceFluxboxCommand::setEditorData(QWidget *editor, const AbstractFluxboxCommand *command)
{
    FC_UI(WorkspaceFluxboxCommandUi);
    FC_CMD(const WorkspaceFluxboxCommand);
    ui->workspaceNumberEditor->setValue(cmd->m_workspaceNumber);
}

void WorkspaceFluxboxCommand::setCommandData(QWidget *editor, AbstractFluxboxCommand *command)
{
    FC_UI(WorkspaceFluxboxCommandUi);
    FC_CMD(WorkspaceFluxboxCommand);
    ui->workspaceNumberEditor->interpretText();
    cmd->m_workspaceNumber = ui->workspaceNumberEditor->value();
}

UnknownFluxboxCommandUi::UnknownFluxboxCommandUi(QWidget *parent)
    : QWidget(parent), editor(new QLineEdit)
{
    setLayout(new QHBoxLayout);
    layout()->addWidget(editor);
}

void UnknownFluxboxCommand::construct()
{
    m_name = "Unknown";
    m_cmd = arg();
}

void UnknownFluxboxCommand::setEditorData(QWidget *editor, const AbstractFluxboxCommand *command)
{
    FC_UI(UnknownFluxboxCommandUi);
    FC_CMD(const UnknownFluxboxCommand);
    ui->editor->setText(cmd->m_cmd);
}

void UnknownFluxboxCommand::setCommandData(QWidget *editor, AbstractFluxboxCommand *command)
{
    FC_UI(UnknownFluxboxCommandUi);
    FC_CMD(UnknownFluxboxCommand);
    cmd->m_cmd = ui->editor->text();
}

TabFluxboxCommandUi::TabFluxboxCommandUi(QWidget *parent)
    : QWidget(parent), tabNumberEditor(new QSpinBox)
{
    setLayout(new QHBoxLayout);
    layout()->addWidget(tabNumberEditor);
}

void TabFluxboxCommand::construct()
{
    bool ok = false;
    int i = arg().toInt(&ok);
    if(!ok) i = 1;
    m_tabNumber = i;
}

void TabFluxboxCommand::setEditorData(QWidget *editor, const AbstractFluxboxCommand *command)
{
    FC_UI(TabFluxboxCommandUi);
    FC_CMD(const TabFluxboxCommand);
    ui->tabNumberEditor->setValue(cmd->m_tabNumber);
}

void TabFluxboxCommand::setCommandData(QWidget *editor, AbstractFluxboxCommand *command)
{
    FC_UI(TabFluxboxCommandUi);
    FC_CMD(TabFluxboxCommand);
    ui->tabNumberEditor->interpretText();
    cmd->m_tabNumber = ui->tabNumberEditor->value();
}

DialogDelegate::DialogDelegate(QObject *parent)
    : QObject(parent)
{
}

KeySequencesDelegateUi::KeySequencesDelegateUi(QWidget *parent)
    : QWidget(parent), ui(new Ui::KeySequencesEdit)
{
    ui->setupUi(this);
    ui->listView_KeySequences->setModel(&model);
    ui->listView_KeySequences->setItemDelegate(&delegate);
    connect(ui->pushButton_Add, &QPushButton::clicked, [this](){
        QStandardItem* item = new QStandardItem;
        QVariant value;
        value.setValue(FluxboxKeySequence());
        item->setData(value, UserRole::DataRole);
        item->setData("Unset", Qt::DisplayRole);
        model.appendRow(item);
    });
    connect(ui->pushButton_Remove, &QPushButton::clicked, [this](){
        QList<int> rows;
        for(QModelIndex index : ui->listView_KeySequences->selectionModel()->selectedRows())
        {
            rows << index.row();
        }
        std::sort(rows.begin(), rows.end(), std::less<int>());
        for(int row : rows)
        {
            model.removeRow(row);
        }
    });
}

KeySequencesDelegateUi::~KeySequencesDelegateUi()
{
    delete ui;
}

FluxboxKeySequenceList KeySequencesDelegateUi::keySequences() const
{
    FluxboxKeySequenceList result;
    for(int i = 0; i < model.rowCount(); i++)
    {
        result << model.item(i)->data(UserRole::DataRole).value<FluxboxKeySequence>();
    }
    FluxboxKeySequenceList result2;
    for(FluxboxKeySequence seq : result)
    {
        if(seq.type() != FluxboxKeySequence::None)
            result2 << seq;
    }
    return result2;
}

void KeySequencesDelegateUi::setKeySequences(const FluxboxKeySequenceList &list)
{
    model.clear();
    for(FluxboxKeySequence seq : list)
    {
        QStandardItem* item = new QStandardItem;
        QVariant value;
        value.setValue(seq);
        item->setData(value, UserRole::DataRole);
        item->setData(seq.toPrettyString(), Qt::DisplayRole);
        model.appendRow(item);
    }
}

void KeySequencesDialogDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    KeySequencesDelegateUi* ui = dynamic_cast<KeySequencesDelegateUi*>(editor);
    if(index.model()->data(index, UserRole::DataRole).canConvert<FluxboxKeySequenceList>())
    {
        FluxboxKeySequenceList list = index.model()->data(index, UserRole::DataRole).value<FluxboxKeySequenceList>();
        ui->setKeySequences(list);
    }
    else
    {
        DEBUG << "Invalid Type";
    }
}

void KeySequencesDialogDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    KeySequencesDelegateUi* ui = dynamic_cast<KeySequencesDelegateUi*>(editor);
    QVariant value;
    value.setValue(ui->keySequences());
    DEBUG << FluxboxKeySequence::listToPrettyString(ui->keySequences());
    model->setData(index, value, UserRole::DataRole);
    model->setData(index, FluxboxKeySequence::listToPrettyString(ui->keySequences()), Qt::DisplayRole);
    //KeysTab::setIndex(model, index, value);
}

CommandDialogDelegateUi::CommandDialogDelegateUi(QWidget *parent)
    : QWidget(parent), m_commandTypeSelector(new QComboBox),
      m_editor(nullptr)
{
    setLayout(new QVBoxLayout);
    layout()->addWidget(m_commandTypeSelector);
    QStringList items = FluxboxCommandInfo::AvailableCommands.keys();
    items.sort();
    m_commandTypeSelector->addItems(items);
    newCommandOn();
}

void CommandDialogDelegateUi::setCommand(AbstractFluxboxCommand *command)
{
    m_command = command;
    newCommandOff();
    m_commandTypeSelector->setCurrentText(m_command->name());
    newCommandOn();
    if(m_editor)
    {
        layout()->removeWidget(m_editor);
        m_editor->deleteLater();
    }
    if(m_command->configurable())
    {
        m_editor = m_command->createEditor(nullptr);
        layout()->addWidget(m_editor);
        m_command->setEditorData(m_editor, m_command);
    }
    else
    {
        m_editor = nullptr;
    }
}

void CommandDialogDelegateUi::newCommandOn()
{
    connect(m_commandTypeSelector, &QComboBox::currentTextChanged, this, &CommandDialogDelegateUi::onTextChanged);
}

void CommandDialogDelegateUi::newCommandOff()
{
    disconnect(m_commandTypeSelector, nullptr, this, nullptr);
}

void CommandDialogDelegateUi::onTextChanged()
{
    setCommand(FluxboxCommandDispatcher::dispatch(m_commandTypeSelector->currentText()));
}

void CommandDialogDelegateUi::onKeySequencesChanged()
{
    if(m_sequences.size() > 1 || (m_sequences.size() > 0 && m_sequences.first().type() != FluxboxKeySequence::Type::Mouse))
    {
        m_commandTypeSelector->clear();
        QStringList items;
        for(auto key : FluxboxCommandInfo::AvailableCommands.keys())
        {
            if(!FluxboxCommandInfo::AvailableCommands[key])
                items << key;
        }
        m_commandTypeSelector->addItems(items);
    }
    else
    {
        m_commandTypeSelector->clear();
        m_commandTypeSelector->addItems(FluxboxCommandInfo::AvailableCommands.keys());
    }
}

void CommandDialogDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.data(UserRole::DataRole).canConvert<AbstractFluxboxCommand*>())
    {
        CommandDialogDelegateUi* ui = dynamic_cast<CommandDialogDelegateUi*>(editor);
        ui->setCommand(index.data(UserRole::DataRole).value<AbstractFluxboxCommand*>());
    }
}

void CommandDialogDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    CommandDialogDelegateUi* ui = dynamic_cast<CommandDialogDelegateUi*>(editor);
    AbstractFluxboxCommand* cmd = ui->command();
    cmd->setCommandData(ui->m_editor, cmd);
    QVariant value;
    value.setValue(cmd);
    model->setData(index, value, UserRole::DataRole);
    model->setData(index, ui->command()->toString(), Qt::DisplayRole);
}
