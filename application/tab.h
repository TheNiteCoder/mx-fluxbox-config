#ifndef TAB_H
#define TAB_H

#include <QString>
#include <QIcon>
#include <QWidget>

#include "fluxboxsource.h"

class Tab
{
public:
    Tab(QString name, QIcon icon = QIcon());
    virtual ~Tab();
    QWidget* widget();
    void setWidget(QWidget* widget);
    QString name() const;
    QIcon icon() const;
    virtual void setup(FluxboxSource::Files source) = 0;
    virtual void apply(FluxboxSource::Files& source) = 0;
signals:
    void needsApply();
private:
    QWidget* m_widget;
    QString m_name;
    QIcon m_icon;
};

#endif // TAB_H
