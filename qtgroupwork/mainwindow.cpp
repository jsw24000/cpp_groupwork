#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QTextStream>

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

    // 自动初始化显示src/posts/里的所有txt文件的内容
    QDir postsDir("src/posts");
    QStringList filters;
    filters << "*.txt";
    QStringList txtFiles = postsDir.entryList(filters, QDir::Files);

    for (const QString &fileName : txtFiles) {
        QString filePath = postsDir.absoluteFilePath(fileName);
        QString content = AddPostDialog::getText(filePath);
        addPost(content);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addPostButton_clicked() //如果点击添加按钮
{
    AddPostDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) { //如果确认了
        dialog.saveTextToFile(dialog.getInputText()); //保存
        QString content = "空空如也";
        QString fileName = "post_" + QString::number(AddPostDialog::totalPostNumber - 1) + ".txt";
        QString filePath = "src/posts/" + fileName;
        content=AddPostDialog::getText(filePath);
        addPost(content);
    }
}

void MainWindow::addPost(const QString &content) //把帖子显示在滚动区域
{
    PostWidget *postWidget = new PostWidget(content, ui->scrollAreaWidgetContents);
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    layout->addWidget(postWidget);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    AddPostDialog::saveTotalPostNumber(); // 关闭时保存计数器
    event->accept();
}
