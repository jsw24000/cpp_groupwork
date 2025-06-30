#include "selfcenterwindow.h"
#include "ui_selfcenterwindow.h"

SelfCenterWindow::SelfCenterWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SelfCenterWindow)
{
    ui->setupUi(this);
}

SelfCenterWindow::~SelfCenterWindow()
{
    delete ui;
}
