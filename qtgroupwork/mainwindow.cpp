#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置滚动区域的布局
    QVBoxLayout *layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(5);
    ui->scrollAreaWidgetContents->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addPostButton_clicked()
{
    AddPostDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // 这里假设 Addpostdialog 有获取帖子标题和内容的方法
        QString content = "Post Content";
        addPost(content);
    }
}

void MainWindow::addPost(const QString &content)
{
    PostWidget *postWidget = new PostWidget(content, ui->scrollAreaWidgetContents);
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    layout->addWidget(postWidget);
}
