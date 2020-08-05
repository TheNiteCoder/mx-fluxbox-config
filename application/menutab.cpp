#include "menutab.h"
#include "ui_menutab.h"

#include <QHBoxLayout>

MenuTab::MenuTab(QObject* parent)
    : Tab("Menu", QIcon::fromTheme("menu"), parent)
{
    setWidget(new QWidget);
    //ui->setupUi(widget());
    widget()->setLayout(new QHBoxLayout);
    widget()->layout()->addWidget(new FluxboxKeySequenceEditWithMouseOptions);
}

MenuTab::~MenuTab()
{
    delete ui;
}
