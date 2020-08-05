#ifndef TAB_H
#define TAB_H

#include <QObject>
#include <QIcon>
#include <QWidget>
#include <QTabWidget>

#include "fluxboxsource.h"
#include "debug.h"

class Tab : public QObject
{
    Q_OBJECT
public:
    explicit Tab(const QString name, const QIcon icon = QIcon(), QObject *parent = nullptr);
    Tab(Tab& copy);
    Tab(Tab&& move);
    Tab& operator = (Tab& copy);
    Tab& operator = (Tab&& move);
    ~Tab();
    Tab& setName(const QString name);
    QString name();
    Tab& setIcon(const QIcon icon);
    QIcon icon();
    Tab& setWidget(QWidget* widget);
    QWidget* widget();

    virtual void setup(FluxboxSource source) = 0;
    virtual void apply(FluxboxSource& source) = 0;

private:
    QString m_name;
    QIcon m_icon;
    QWidget* m_widget;
};

class TabManager : public QObject
{
    Q_OBJECT
public:
    TabManager(QList<Tab*> tabs = QList<Tab*>(), QTabWidget* tabWidget = nullptr, QObject* parent = nullptr);
    void registerTab(Tab* tab);
    void setup(FluxboxSource source);
    void apply(FluxboxSource& source);
    void setupTabWidget(QTabWidget* tabWidget);
private:
    QList<Tab*> m_tabs;
};

#endif // TAB_H
