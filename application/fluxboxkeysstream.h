#ifndef FLUXBOXKEYSSTREAM_H
#define FLUXBOXKEYSSTREAM_H

#include <QString>

#include "fluxboxcommands.h"
#include "fluxboxkeysequence.h"

class FluxboxKeysStream
{
public:
    FluxboxKeysStream(QString* device);
    enum CompareMethod
    {
        PreferKeys,
        PreferCommand,
    };

    struct Mapping
    {
        int line;
        FluxboxKeySequenceList sequences;
        AbstractFluxboxCommand* command;
        bool operator==(const Mapping& other);
    };

    using MappingList = QList<Mapping>;

    MappingList parseMappings() const;

    void setMappings(MappingList parseMappings);
    void setMapping(FluxboxKeySequenceList sequences, AbstractFluxboxCommand* command, CompareMethod method = PreferKeys);
    void setMappingOnLine(FluxboxKeySequenceList sequences, AbstractFluxboxCommand* command, int line);
    void appendMapping(FluxboxKeySequenceList sequences, AbstractFluxboxCommand* command);
    QString& device() { return *m_device; }
    QString device() const { return *m_device; }
    static QString removeCommentFromLine(QString line);
private:
    void removeExtras(MappingList parseMappings);
    void removeLine(int line);
    QString* m_device;
};

#endif // FLUXBOXKEYSSTREAM_H
