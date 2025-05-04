#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loginWidget = new LoginWidget(this);
    postWidget = new PostWidget(this);
    // 初始化界面和信号槽
}

MainWindow::~MainWindow()
{
    delete ui;
}