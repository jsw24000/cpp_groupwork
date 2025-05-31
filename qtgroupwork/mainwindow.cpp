#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addpostdialog.h"
#include "postwidget.h"
#include "client.h"
#include <client.h>
#include <QMessageBox>
#include <QCloseEvent>

#include <QMap>
#include <QStringList>
#include <QSizePolicy>
#include <QSize>
#include <QLineEdit>
#include <QLayoutItem>

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

    connect(&client, &Client::postsReceived, this, &MainWindow::handlePostsReceived);
    connect(&client, &Client::dataReceived, this, &MainWindow::handleDataReceived);
    connect(&client, &Client::errorOccurred, this, &MainWindow::handleSocketError);

    QLabel *loadingLabel = new QLabel("正在加载帖子...", ui->scrollAreaWidgetContents);
    loadingLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(loadingLabel);

    if (client.connectToServer("192.168.43.242", 1234)) {
        loadAllPosts();
    } else {
        handleConnectionError();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addPostButton_clicked()
{
    AddPostDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString content = dialog.getInputText();

        if (client.connectToServer("192.168.43.242", 1234)) {
            client.sendPost(content);

            QLabel *sendingLabel = new QLabel("正在发送帖子...", ui->scrollAreaWidgetContents);
            sendingLabel->setAlignment(Qt::AlignCenter);

            connect(&client, &Client::dataReceived, [this](const QByteArray& data) {
                QString response = QString(data);
                if (response == "Success") {
                    QMessageBox::information(this, "发布成功", "文本已成功发布了！");
                } else {
                    QMessageBox::critical(this, "发布失败", "糟糕！请再试试看");
                }
            });

            ui->scrollAreaWidgetContents->layout()->addWidget(sendingLabel);

            loadAllPosts();
        } else {
            QMessageBox::critical(this, "网络错误", "无法连接到服务器，请检查网络设置");
        }
    }
}

void MainWindow::addPost(const QString &content, const QString &fileName)
{
    PostWidget *postWidget = new PostWidget(content, fileName, ui->scrollAreaWidgetContents);
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());

    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (QLabel* label = qobject_cast<QLabel*>(item->widget())) {
            delete label;
        } else {
            layout->insertItem(0, item);
            break;
        }
        delete item;
    }

    layout->addWidget(postWidget);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    AddPostDialog::saveTotalPostNumber(); // 关闭时保存计数器
    event->accept();
}

void MainWindow::loadAllPosts()
{
    client.requestAllPosts();
}

void MainWindow::handlePostsReceived(const QList<Post>& posts)
{
    QLayoutItem *child;
    while ((child = ui->scrollAreaWidgetContents->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    if (posts.isEmpty()) {
        QLabel *emptyLabel = new QLabel("暂无帖子", ui->scrollAreaWidgetContents);
        emptyLabel->setAlignment(Qt::AlignCenter);
        ui->scrollAreaWidgetContents->layout()->addWidget(emptyLabel);
    } else {
        for (const Post& post : posts) {
            addPost(post.content(), post.fileName());
        }
    }
}

void MainWindow::handleDataReceived(const QByteArray& data)
{
    qDebug() << "Received raw data:" << data;
}

void MainWindow::handleSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error:" << client.m_socket->errorString(); // 通过 m_socket 调用 errorString 方法
    handleConnectionError();
}

void MainWindow::handleConnectionError()
{
    QLayoutItem *child;
    while ((child = ui->scrollAreaWidgetContents->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QLabel *errorLabel = new QLabel("无法连接到服务器\n请检查网络连接或服务器状态", ui->scrollAreaWidgetContents);
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setStyleSheet("color: red; font-size: 14pt;");
    ui->scrollAreaWidgetContents->layout()->addWidget(errorLabel);

    QPushButton *retryButton = new QPushButton("重试", ui->scrollAreaWidgetContents);
    connect(retryButton, &QPushButton::clicked, this, [this]() {
        loadAllPosts();
    });
    ui->scrollAreaWidgetContents->layout()->addWidget(retryButton);
}


QMap<QString, QWidget*> originalWidgets;
bool isEditable = false;

void MainWindow::on_selfcenterButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_menuButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}



void MainWindow::on_infoEditButton_clicked()
{
    isEditable = !isEditable;
    const QStringList fields = {"username", "password", "number", "school", "phone"};
    QPushButton* infoEdit = findChild<QPushButton*>("infoEditButton");
    if (infoEdit) infoEdit->setText(isEditable ? "提交修改" : "更改信息"); // 让按钮根据状态改变text信息


    for (const QString& field : fields) {
        const QString labelName = field + "Label";
        const QString lineEditName = field + "LineEdit";

        if (isEditable) { // 进入编辑模式：替换Label为LineEdit
            QLabel* originalLabel = findChild<QLabel*>(labelName); // 找到对应的QLabel
            if (!originalLabel || originalWidgets.contains(field)) continue;

            // 保存当前QLabel的原始状态（到相应QMap中）
            originalWidgets[field] = originalLabel;
            originalSizePolicies[field] = originalLabel->sizePolicy();
            originalSizeHints[field] = originalLabel->sizeHint();

            // 创建LineEdit并继承Label的尺寸属性
            QLineEdit* edit = new QLineEdit(originalLabel->text());
            edit->setObjectName(lineEditName);
            edit->setSizePolicy(originalSizePolicies[field]); // 统一尺寸策略
            edit->setMinimumSize(originalSizeHints[field]);   // 统一最小尺寸

            // 特殊处理密码字段（变为不可见）
            if (field == "password") {
                edit->setEchoMode(QLineEdit::Password);
            }

            // 替换控件并隐藏原始Label
            ui->gridLayout->replaceWidget(originalLabel, edit);
            originalLabel->hide();
        } else { // 提交修改：替换LineEdit为Label
            QLineEdit* edit = findChild<QLineEdit*>(lineEditName);
            QLabel* originalLabel = originalWidgets.value(field); // 从成员变量获取
            if (!edit || !originalLabel) continue;

            // 更新Label文本并恢复原始尺寸属性
            originalLabel->setText(edit->text());
            originalLabel->setSizePolicy(originalSizePolicies[field]);
            originalLabel->setMinimumSize(originalSizeHints[field]);

            if(field == "password") originalLabel->setText("********");
            // 替换控件并销毁LineEdit
            ui->gridLayout->replaceWidget(edit, originalLabel);
            originalLabel->show();
            edit->deleteLater();  // 彻底移除LineEdit

            // 清理临时存储
            originalWidgets.remove(field);
            originalSizePolicies.remove(field);
            originalSizeHints.remove(field);
        }
    }
}
