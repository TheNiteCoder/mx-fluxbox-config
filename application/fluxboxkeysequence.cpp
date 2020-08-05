
#include "fluxboxkeysequence.h"
#include "debug.h"

FluxboxKeySequence::FluxboxKeySequence()
{
    unset();
}

FluxboxKeySequence::FluxboxKeySequence(Qt::KeyboardModifiers modifiers, int keyMouse, FluxboxKeySequence::Type type)
{
    setModifiers(modifiers);
    if(type == Type::Key)
        setKey(static_cast<Qt::Key>(keyMouse));
    else if(type == Type::Mouse)
        setMouseButton(toFluxboxMouseButton(static_cast<Qt::MouseButton>(keyMouse)));
}

void FluxboxKeySequence::setKey(Qt::Key key)
{
    m_type = Type::Key;
    m_key = key;
}

void FluxboxKeySequence::setMouseButton(FluxboxMouseButton button)
{
    m_type = Type::Mouse;
    m_mouseButton = button;
}

void FluxboxKeySequence::setModifiers(Qt::KeyboardModifiers modifiers)
{
    m_keyboardModifiers = modifiers;
}

void FluxboxKeySequence::setMouseModifiers(FluxboxMouseModifiers modifiers)
{
    m_type = Type::Mouse;
    m_mouseModifiers = modifiers;
}

QString FluxboxKeySequence::toString() const
{
    if(m_type == Type::Key)
    {
        QList<int> modifiers;
        if(m_keyboardModifiers & Qt::ShiftModifier) modifiers << Qt::SHIFT;
        if(m_keyboardModifiers & Qt::ControlModifier) modifiers << Qt::CTRL;
        if(m_keyboardModifiers & Qt::MetaModifier) modifiers << Qt::META;
        if(m_keyboardModifiers & Qt::AltModifier) modifiers << Qt::ALT;
        modifiers << m_key;
        QKeySequence seq{std::accumulate(modifiers.begin(), modifiers.end(), 0)};
        return keyString({seq});
    }
    else if(m_type == Type::Mouse)
    {
        QStringList list;
        if(m_mouseModifiers & FluxboxMouseModifier::OnTab) list << "OnTab";
        if(m_mouseModifiers & FluxboxMouseModifier::OnWindow) list << "OnWindow";
        if(m_mouseModifiers & FluxboxMouseModifier::OnTitlebar) list << "OnTitlebar";
        if(m_mouseModifiers & FluxboxMouseModifier::OnToolbar) list << "OnToolbar";
        if(m_mouseModifiers & FluxboxMouseModifier::OnDesktop) list << "OnDesktop";
        if(m_mouseModifiers & FluxboxMouseModifier::Double) list << "Double";
        if(m_mouseModifiers & FluxboxMouseModifier::OnSlit) list << "OnSlit";
        if(m_mouseModifiers & FluxboxMouseModifier::OnLeftGrip) list << "OnLeftGrip";
        if(m_mouseModifiers & FluxboxMouseModifier::OnRightGrip) list << "OnRightGrip";
        if(m_mouseModifiers & FluxboxMouseModifier::OnIconButton) list << "OnIconButton";
        if(m_mouseModifiers & FluxboxMouseModifier::OnWindowBorder) list << "OnWindowBorder";
        if(m_keyboardModifiers == Qt::NoModifier) list << "none";
        if(m_keyboardModifiers & Qt::ShiftModifier) list << "Shift";
        if(m_keyboardModifiers & Qt::ControlModifier) list << "Control";
        if(m_keyboardModifiers & Qt::MetaModifier) list << "Mod4";
        if(m_keyboardModifiers & Qt::AltModifier) list << "Mod1";

        QString result;
        if(m_mouseEventType == FluxboxMouseEventType::Move)
            result = "Move";
        else if(m_mouseEventType == FluxboxMouseEventType::Click)
            result = "Click";
        else if(m_mouseEventType == FluxboxMouseEventType::Press)
            result = "Mouse";
        else
            result = "UnknownMouseEvent";

        result += QString::number(static_cast<int>(mouseButton()));

        list << result;

        return list.join(' ');
    }
    return {};
}

QString FluxboxKeySequence::toPrettyString() const
{
    if(type() == Type::None)
    {
        return {""};
    }
    QString result;
    if(type() == Type::Mouse)
    {
        QStringList parts;
#define C(e) if(mouseModifiers() & FluxboxMouseModifier::e) parts << #e
        C(OnTab);
        C(OnTitlebar);
        C(OnWindow);
        C(OnDesktop);
        C(OnToolbar);
        C(Double);
        C(OnLeftGrip);
        C(OnRightGrip);
        C(OnIconButton);
        C(OnWindowBorder);
#undef C
        result += parts.join(' ');
        if(result.size() > 0)
            result += ' ';
    }
    QStringList modifiers;
    if(keyboardModifiers() & Qt::ControlModifier) modifiers << "Control";
    if(keyboardModifiers() & Qt::AltModifier) modifiers << "Alt";
    if(keyboardModifiers() & Qt::MetaModifier) modifiers << "Meta";
    if(keyboardModifiers() & Qt::ShiftModifier) modifiers << "Shift";
    if(type() == Type::Key)
    {
        QKeySequence seq{static_cast<Qt::Key>(key())};
        QString keyName = seq.toString();
        modifiers << keyName;
    }
    else if(type() == Type::Mouse)
    {
        QString mouseEventName;
        if(mouseEventType() == FluxboxMouseEventType::Press)
            mouseEventName = "Press";
        else if(mouseEventType() == FluxboxMouseEventType::Click)
            mouseEventName = "Click";
        else if(mouseEventType() == FluxboxMouseEventType::Move)
            mouseEventName = "Move";
        QString mouseName;
        if(mouseButton() == FluxboxMouseButton::Button1)
            mouseName = "Left";
        else if(mouseButton() == FluxboxMouseButton::Button2)
            mouseName = "Middle";
        else if(mouseButton() == FluxboxMouseButton::Button3)
            mouseName = "Right";
        else if(mouseButton() == FluxboxMouseButton::Button4)
            mouseName = "ScrollUp";
        else if(mouseButton() == FluxboxMouseButton::Button5)
            mouseName = "ScrollDown";
        modifiers << mouseEventName + ' ' + mouseName;
    }
    result += modifiers.join('+');
    return result;
}

FluxboxKeySequence FluxboxKeySequence::fromString(QString string)
{
    QList<FluxboxKeySequence> list = listFromString(string);
    if(list.size() > 0) return list[0];
    return FluxboxKeySequence{};
}

QList<FluxboxKeySequence> FluxboxKeySequence::listFromString(QString string)
{
    QList<FluxboxKeySequence> results;
    QStringList parts = string.split(' ', QString::SkipEmptyParts);
    std::for_each(parts.begin(), parts.end(), [](QString& in){
        in = in.trimmed();
    });
    int pos = 0;
    QStringList mouseModifiers = {"ondesktop", "ontoolbar", "oniconbutton",
                                  "ontitlebar", "onwindow", "onwindowborder",
                                  "onleftgrip", "onrightgrip" , "ontab", "onslit", "double"};
    while(pos < parts.size() )//- 1)
    {
        Qt::KeyboardModifiers modifiers = Qt::KeyboardModifier::NoModifier;
        FluxboxMouseModifiers mModifiers = FluxboxMouseModifier::NoModifier;
        FluxboxKeySequence result;
        while(pos < parts.size()) //- 1)
        {
            if(!modifierKeys.contains(parts[pos], Qt::CaseInsensitive) &&
                    !mouseModifiers.contains(parts[pos], Qt::CaseInsensitive))
            {
                break;
            }
            if(!parts[pos].contains("none", Qt::CaseInsensitive))
            {
                if(parts[pos].contains("control", Qt::CaseInsensitive))
                    modifiers |= Qt::ControlModifier;
                else if(parts[pos].contains("mod1", Qt::CaseInsensitive))
                    modifiers |= Qt::AltModifier;
                else if(parts[pos].contains("alt", Qt::CaseInsensitive))
                    modifiers |= Qt::AltModifier;
                else if(parts[pos].contains("mod4", Qt::CaseInsensitive))
                    modifiers |= Qt::MetaModifier;
                else if(parts[pos].contains("meta", Qt::CaseInsensitive))
                    modifiers |= Qt::MetaModifier;
                else if(parts[pos].contains("shift", Qt::CaseInsensitive))
                    modifiers |= Qt::ShiftModifier;
                else if(parts[pos].contains("ondesktop", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnDesktop;
                else if(parts[pos].contains("ontoolbar", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnToolbar;
                else if(parts[pos].contains("onwindow", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnWindow;
                else if(parts[pos].contains("ontitlebar", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnTitlebar;
                else if(parts[pos].contains("ontab", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnTab;
                else if(parts[pos].contains("double", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::Double;
                else if(parts[pos].contains("oniconbutton", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnIconButton;
                else if(parts[pos].contains("onslit", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnSlit;
                else if(parts[pos].contains("onleftgrip", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnLeftGrip;
                else if(parts[pos].contains("onrightgrip", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnRightGrip;
                else if(parts[pos].contains("onwindowborder", Qt::CaseInsensitive))
                    mModifiers |= FluxboxMouseModifier::OnWindowBorder;
            }
            else
            {
                modifiers = Qt::NoModifier;
            }
            ++pos;
        }
        if(parts[pos].indexOf("Mouse") == 0)
        {
            result.setMouseEventType(FluxboxMouseEventType::Press);
            if(parts[pos].contains("Mouse1", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button1);
            else if(parts[pos].contains("Mouse2", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button2);
            else if(parts[pos].contains("Mouse3", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button3);
            else if(parts[pos].contains("Mouse4", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button4);
            else if(parts[pos].contains("Mouse5", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button5);
            pos++;
        }
        else if(parts[pos].indexOf("Move") == 0)
        {
            result.setMouseEventType(FluxboxMouseEventType::Move);
            if(parts[pos].contains("Move1", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button1);
            else if(parts[pos].contains("Move2", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button2);
            else if(parts[pos].contains("Move3", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button3);
            else if(parts[pos].contains("Move4", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button4);
            else if(parts[pos].contains("Move5", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button5);
            pos++;
        }
        else if(parts[pos].indexOf("Click") == 0)
        {
            result.setMouseEventType(FluxboxMouseEventType::Click);
            if(parts[pos].contains("Click1", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button1);
            else if(parts[pos].contains("Click2", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button2);
            else if(parts[pos].contains("Click3", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button3);
            else if(parts[pos].contains("Click4", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button4);
            else if(parts[pos].contains("Click5", Qt::CaseInsensitive)) result.setMouseButton(FluxboxMouseButton::Button5);
            pos++;
        }
        else
        {
            QKeySequence sequence = QKeySequence::fromString(parts[pos]);
            Q_ASSUME(sequence.count() == 1);
            Qt::Key key = static_cast<Qt::Key>(sequence[0]);
            result.setKey(key);
            pos++;
        }
        result.setModifiers(modifiers);
        if(mModifiers != FluxboxMouseModifier::NoModifier)
            result.setMouseModifiers(mModifiers);
        results << result;
    }
    return results;
}

QString FluxboxKeySequence::listToString(QList<FluxboxKeySequence> sequences)
{
    QStringList collection;
    for(FluxboxKeySequence seq : sequences)
        collection << seq.toString();
    return collection.join(' ');
}

QString FluxboxKeySequence::listToPrettyString(QList<FluxboxKeySequence> sequences)
{
    QStringList parts;
    for(auto seq : sequences)
    {
        parts << seq.toPrettyString();
    }
    return parts.join(", ");
}

bool FluxboxKeySequence::operator==(const FluxboxKeySequence &other)
{
    if(other.type() == Type::None && type() == Type::None)
        return true;
    if(other.type() == Type::Key && type() == Type::Key)
        return other.key() == key() && other.keyboardModifiers() == keyboardModifiers();
    if(other.type() == Type::Mouse && type() == Type::Mouse)
        return other.mouseButton() == mouseButton() &&
                other.mouseEventType() == mouseEventType() &&
                other.mouseModifiers() == mouseModifiers() &&
                other.keyboardModifiers() == keyboardModifiers();
    return false;
}

bool FluxboxKeySequence::operator!=(const FluxboxKeySequence &other)
{
    return !(*this == other);
}

void FluxboxKeySequence::unset()
{
    setModifiers(Qt::KeyboardModifier::NoModifier);
    m_type = Type::None;
    m_mouseButton = FluxboxMouseButton::ButtonUnknown;
    m_mouseEventType = FluxboxMouseEventType::Unknown;
    m_mouseModifiers = FluxboxMouseModifier::NoModifier;
}

QString FluxboxKeySequence::keyString(QList<QKeySequence> sequences)
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

QString FluxboxKeySequence::replaceKeyNames(QString str, bool reverse)
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

QString FluxboxKeySequence::addNones(QString string)
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

FluxboxMouseButton FluxboxKeySequence::toFluxboxMouseButton(Qt::MouseButton button)
{
    switch(button)
    {
#define C(t, r) case Qt::MouseButton::t: return FluxboxMouseButton::r
    C(LeftButton, Button1);
    C(MiddleButton, Button2);
    C(RightButton, Button3);
#undef C
    default: return FluxboxMouseButton::ButtonUnknown;
    }
}
