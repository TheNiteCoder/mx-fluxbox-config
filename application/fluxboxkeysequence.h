#ifndef FLUXBOXKEYSEQUENCE_H
#define FLUXBOXKEYSEQUENCE_H

#include <QFlags>
#include <QWidget>
#include <QMap>

const QStringList modifierKeys = {"Ctrl", "Mod1", "Mod4", "Alt", "Meta", "Shift", "Control", "None"};
const QMap<QString, QString> keyReplacement = {
    {"Control", "Ctrl"},
    {"Mod1", "Alt"},
    {"Mod4", "Meta"}
};


enum class FluxboxMouseButton
{
    Button1 = 1, // start at 1 so it can be easily converted to string
    Button2,
    Button3,
    Button4,
    Button5,
    ButtonUnknown
};

enum class FluxboxMouseEventType
{
    Click,
    Move,
    Press, // Really is Mouse
    DoubleClick,
    Unknown
};
#define B(n) (1 << n)
enum FluxboxMouseModifier
{
    NoModifier = 0,
    OnDesktop = B(0),
    OnToolbar = B(1),
    OnIconButton = B(2),
    OnTitlebar = B(3),
    OnWindow = B(4),
    OnWindowBorder = B(5),
    OnLeftGrip = B(6),
    OnRightGrip = B(7),
    OnTab = B(8),
    OnSlit = B(9),
    Double = B(10),
};
#undef B

Q_DECLARE_FLAGS(FluxboxMouseModifiers, FluxboxMouseModifier)
Q_DECLARE_OPERATORS_FOR_FLAGS(FluxboxMouseModifiers)

class FluxboxKeySequence
{
public:
    enum Type
    {
        Key,
        Mouse,
        None
    };

    FluxboxKeySequence();
    FluxboxKeySequence(Qt::KeyboardModifiers modifiers, int keyMouse, Type type);
    FluxboxKeySequence(const FluxboxKeySequence& other) = default;
    void setKey(Qt::Key key);
    void setMouseButton(FluxboxMouseButton button);
    void setMouseEventType(FluxboxMouseEventType type) { m_mouseEventType = type; }
    void setModifiers(Qt::KeyboardModifiers modifiers);
    void setMouseModifiers(FluxboxMouseModifiers modifiers);
    Type type() const { return m_type; }
    FluxboxMouseButton mouseButton() const { return m_mouseButton; }
    Qt::Key key() const { return m_key; }
    Qt::KeyboardModifiers keyboardModifiers() const { return m_keyboardModifiers; }
    FluxboxMouseModifiers mouseModifiers() const { return m_mouseModifiers; }
    FluxboxMouseEventType mouseEventType() const { return m_mouseEventType; }
    QString toString() const;
    QString toPrettyString() const;
    static FluxboxKeySequence fromString(QString string);
    static QList<FluxboxKeySequence> listFromString(QString string);
    static QString listToString(QList<FluxboxKeySequence> sequences);
    static QString listToPrettyString(QList<FluxboxKeySequence> sequences);
    bool operator==(const FluxboxKeySequence& other);
    bool operator!=(const FluxboxKeySequence& other);
    void unset();
    operator QString() { return toString(); }
    static FluxboxMouseButton toFluxboxMouseButton(Qt::MouseButton button);
private:
    static QString keyString(QList<QKeySequence> sequences);
    static QString replaceKeyNames(QString str, bool reverse = false);
    static QString addNones(QString string);
    Type m_type;
    Qt::Key m_key;
    FluxboxMouseButton m_mouseButton;
    Qt::KeyboardModifiers m_keyboardModifiers;
    FluxboxMouseModifiers m_mouseModifiers;
    FluxboxMouseEventType m_mouseEventType;
};

Q_DECLARE_METATYPE(FluxboxKeySequence)

using FluxboxKeySequenceList = QList<FluxboxKeySequence>;

#endif // FLUXBOXKEYSEQUENCE_H
