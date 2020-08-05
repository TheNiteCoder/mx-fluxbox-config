#include "fluxboxkeysstream.h"

#include "debug.h"

FluxboxKeysStream::FluxboxKeysStream(QString* device)
    : m_device(device)
{
}

FluxboxKeysStream::MappingList FluxboxKeysStream::parseMappings() const
{
    MappingList result;
    int lineNumber = -1;
    for(QString line : device().split('\n'))
    {
        lineNumber++;
        line = removeCommentFromLine(line);
        QStringList parts = line.split(':');
        if(parts.size() < 2) continue;
        QString keySequenceString = parts.at(0);
        QString commandString = parts.mid(1).join(':');
        Mapping mapping;
        mapping.sequences = FluxboxKeySequence::listFromString(keySequenceString);
        mapping.command = FluxboxCommandDispatcher::dispatch(commandString);
        if(mapping.command == nullptr)
            continue;
        mapping.line = lineNumber;
        result << mapping;
    }
    return result;
}

void FluxboxKeysStream::setMappings(FluxboxKeysStream::MappingList mappings)
{
    for(auto mapping : mappings)
    {
        setMapping(mapping.sequences, mapping.command);
    }
    removeExtras(mappings);
}

void FluxboxKeysStream::setMapping(FluxboxKeySequenceList sequences, AbstractFluxboxCommand *command, FluxboxKeysStream::CompareMethod method)
{
    Q_UNUSED(method)
#define L(l) Timer _timer_; l std::cerr << _timer_.elapsed() << std::endl;
#undef L
    MappingList mappings = parseMappings();
    bool set = false;
    for(Mapping mapping : mappings)
    {
        bool first = sequences == mapping.sequences;
        if(first)
        {
            setMappingOnLine(sequences, command, mapping.line);
            set = true;
            break;
        }
    }
    if(!set) appendMapping(sequences, command);
}

void FluxboxKeysStream::setMappingOnLine(FluxboxKeySequenceList sequences, AbstractFluxboxCommand *command, int line)
{
    QStringList lines = device().split('\n');
    lines[line] = FluxboxKeySequence::listToString(sequences) + " :" + command->toString();
    device() = lines.join('\n');
}

void FluxboxKeysStream::appendMapping(FluxboxKeySequenceList sequences, AbstractFluxboxCommand *command)
{
    QStringList lines = device().split('\n');
    lines << FluxboxKeySequence::listToString(sequences) + " :" + command->toString();
    device() = lines.join('\n');
}

QString FluxboxKeysStream::removeCommentFromLine(QString line)
{
    int pos = (line.contains('#') ? line.indexOf('#') : line.indexOf('!'));
    if(pos == -1) return line;
    line = line.mid(0, pos);
    return line;
}

void FluxboxKeysStream::removeExtras(FluxboxKeysStream::MappingList mappings)
{
    MappingList parsed = parseMappings();
    int offset = 0;
    for(Mapping check : parsed)
    {
        if(!mappings.contains(check))
        {
            DEBUG << "Removing:" << FluxboxKeySequence::listToPrettyString(check.sequences) << ":" << check.command->toString();
            removeLine(check.line - offset);
            offset++;
        }
    }
}

void FluxboxKeysStream::removeLine(int line)
{
    QStringList lines = device().split('\n');
    lines.removeAt(line);
    device() = lines.join('\n');
}

bool FluxboxKeysStream::Mapping::operator==(const FluxboxKeysStream::Mapping &other)
{
    return sequences == other.sequences && command->equal(other.command);
}
