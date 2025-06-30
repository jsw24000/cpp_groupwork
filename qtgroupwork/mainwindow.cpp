#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addpostdialog.h"
#include "postwidget.h"
#include "client.h"
#include <client.h>
#include <QMessageBox>
#include <QCloseEvent>

#include <QMap>
#include <QIcon>
#include <QString>
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

    ui->addPostButton->setFixedSize(50, 50);

    setIcon(35, ui->addPostButton, ":/icon/addpost.png");
    setIcon(50, ui->menuButton, ":/icon/menu.png");
    setIcon(50, ui->selfcenterButton, ":/icon/selfcenter.png");
    setIcon(35, ui->infoEditButton, ":/icon/editinfo.png");


    // 设置滚动区域的布局
    setWindowTitle("北大树坑");
    QVBoxLayout *layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(5);
    ui->scrollAreaWidgetContents->setLayout(layout);

    // 设置个人中心滚动区域的布局
    favoritesLayout = new QVBoxLayout(ui->scrollAreaWidgetContents_2);
    favoritesLayout->setAlignment(Qt::AlignTop);
    favoritesLayout->setSpacing(5);
    ui->scrollAreaWidgetContents_2->setLayout(favoritesLayout);

    connect(&client, &Client::dataReceived, this, &MainWindow::handleUserInfoResponse);
    connect(&client, &Client::postsReceived, this, &MainWindow::handlePostsReceived);
    connect(&client, &Client::favoritesReceived, this, &MainWindow::handleFavoritesReceived);
    connect(&client, &Client::dataReceived, this, &MainWindow::handleDataReceived);
    connect(&client, &Client::errorOccurred, this, &MainWindow::handleSocketError);
    connect(&client, &Client::userInfoReceived, this, [this](const QJsonObject &userInfo) {
        // 直接在这里处理或者调用现有函数
        QJsonDocument doc(userInfo);
        handleUserInfoResponse(doc.toJson());
    });

    QLabel *loadingLabel = new QLabel("正在加载帖子...", ui->scrollAreaWidgetContents);
    loadingLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(loadingLabel);

    if (!client.isConnected()){
        if (!client.connectToServer(qApp->property("IP").toString(), 1234)){
            handleConnectionError();
        }
    }
    qDebug()<<"初始化";

    connect(&client, &Client::dataReceived, [this](const QByteArray& data) {
        QString response = QString(data);
        if(response == "FavoriteSuccess"){
            return;
        }
        if (response == "Success") {
            QMessageBox::information(this, "发布成功", "文本已成功发布了！");
            loadAllPosts();
            qDebug()<<"已加载";
        }
    });
    loadAllPosts();

    // 清空默认用户名
    ui->usernameLabel->setText("");
}

void MainWindow::handleUserInfoResponse(const QByteArray& data)
{
    qDebug()<<"收到user信息";
    QString response = QString(data);
    if (response == "UpdateUserInfoSuccess") {
        QMessageBox::information(this, "成功", "用户信息更新成功");
        client.requestUserInfo(myusername);
    } else if (response == "UpdateUserInfoFailed") {
        QMessageBox::warning(this, "错误", "用户信息更新失败");
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull() && doc.isObject() && doc.object()["type"].toString() == "USERINFO") {
            QJsonObject userInfo = doc.object();
            ui->usernameLabel->setText(userInfo["username"].toString());
            // 直接显示密码明文
            currentPassword = userInfo["password"].toString();
            ui->passwordLabel->setText(currentPassword);
            ui->numberLabel->setText(userInfo["number"].toString());
            ui->schoolLabel->setText(userInfo["school"].toString());
            ui->phoneLabel->setText(userInfo["phone"].toString());
            qDebug()<<"信息如下";
            qDebug()<<userInfo["password"];
            qDebug()<<userInfo["number"];
            qDebug()<<userInfo["school"];
            qDebug()<<userInfo["phone"];
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

//设置用户名
void MainWindow::setUserName(const QString& username){
    myusername=username;
    ui->usernameLabel->setText(username); // 更新UI显示
    client.requestUserInfo(username); // 立即请求用户信息
}

void MainWindow::setIcon(int a, QPushButton* button, const QString& filename){
    button->setText("");
    button->setFixedSize(a, a);
    button->setIcon(QIcon(filename));
    button->setIconSize(QSize(button->width(), button->height()));
    button->setStyleSheet(R"(
        QPushButton {
            background-color: transparent; /* 透明背景 */
            border: none; /* 去除边框 */
            padding: 0px; /* 去除内边距，让图标贴合按钮边缘 */
        })"
    );
}

void MainWindow::on_addPostButton_clicked()
{
    AddPostDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString content = dialog.getInputText();

        if (!client.isConnected()) {
            if (!client.connectToServer(qApp->property("IP").toString(), 1234)) {
                QMessageBox::critical(this, "网络错误", "无法连接到服务器，请检查网络设置");
                return;
            }
        }

        client.sendPost(content);

        QLabel *sendingLabel = new QLabel("正在发送帖子...", ui->scrollAreaWidgetContents);
        sendingLabel->setAlignment(Qt::AlignCenter);


        ui->scrollAreaWidgetContents->layout()->addWidget(sendingLabel);

    }
}

//把帖子显示到滚动界面上
void MainWindow::addPost(const QString &content, const QString &fileName)
{
    PostWidget *postWidget = new PostWidget(content, fileName, ui->scrollAreaWidgetContents);
    postWidget->myusername = this->myusername;
    postWidget->client_ptr = &(this->client);
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

void MainWindow::addPostToFavorites(const QString &content, const QString &fileName)
{
    PostWidget *postWidget = new PostWidget(content, fileName, ui->scrollAreaWidgetContents_2);
    postWidget->myusername = this->myusername;
    postWidget->client_ptr = &(this->client);
    favoritesLayout->addWidget(postWidget);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    AddPostDialog::saveTotalPostNumber(); // 关闭时保存计数器
    event->accept();
}

void MainWindow::loadAllPosts()
{
    client.requestAllPosts();
    qDebug()<<"准备请求全部帖子！";
}

//处理接收到的帖子（要显示在主页面上的）
void MainWindow::handlePostsReceived(const QList<Post>& posts)
{
    QLayoutItem *child;
    while ((child = ui->scrollAreaWidgetContents->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    qDebug()<<"已删除";

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

void MainWindow::handleFavoritesReceived(const QList<Post>& posts)
{
    QLayoutItem *child;
    while ((child = favoritesLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    qDebug()<<"已删除收藏帖子";

    if (posts.isEmpty()) {
        QLabel *emptyLabel = new QLabel("暂无收藏帖子", ui->scrollAreaWidgetContents_2);
        emptyLabel->setAlignment(Qt::AlignCenter);
        favoritesLayout->addWidget(emptyLabel);
    } else {
        for (const Post& post : posts) {
            addPostToFavorites(post.content(), post.fileName());
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
    if (!myusername.isEmpty()) {
        client.requestUserInfo(myusername);
    }
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

    // 更新按钮图标
    if (infoEdit) {
        setIcon(35, infoEdit, isEditable ? ":/icon/submit.png" : ":/icon/editinfo.png");
    }

    for (const QString& field : fields) {
        const QString labelName = field + "Label";
        const QString lineEditName = field + "LineEdit";

        if (isEditable) {
            // 进入编辑模式：替换Label为LineEdit
            QLabel* originalLabel = findChild<QLabel*>(labelName);
            if (!originalLabel || originalWidgets.contains(field)) continue;

            // 保存原始状态
            originalWidgets[field] = originalLabel;
            originalSizePolicies[field] = originalLabel->sizePolicy();
            originalSizeHints[field] = originalLabel->sizeHint();

            // 创建LineEdit
            QLineEdit* edit = new QLineEdit(originalLabel->text());
            edit->setObjectName(lineEditName);
            edit->setSizePolicy(originalSizePolicies[field]);
            edit->setMinimumSize(originalSizeHints[field]);

            // 特殊处理密码字段
            if (field == "password") {
                edit->setText(currentPassword); // 显示真实密码
                edit->setEchoMode(QLineEdit::Normal); // 明文显示
            }

            // 用户名不可编辑
            if (field == "username") {
                edit->setReadOnly(true);
            }

            // 替换控件
            ui->gridLayout->replaceWidget(originalLabel, edit);
            originalLabel->hide();
        } else {
            // 提交修改：替换LineEdit为Label
            QLineEdit* edit = findChild<QLineEdit*>(lineEditName);
            QLabel* originalLabel = originalWidgets.value(field);
            if (!edit || !originalLabel) continue;

            // 更新Label文本
            QString newText = edit->text();
            if (field == "password") {
                currentPassword = edit->text(); // 保存新密码
                originalLabel->setText(currentPassword); // 显示明文密码
            }
            originalLabel->setText(newText);

            // 恢复原始尺寸属性
            originalLabel->setSizePolicy(originalSizePolicies[field]);
            originalLabel->setMinimumSize(originalSizeHints[field]);

            // 替换控件
            ui->gridLayout->replaceWidget(edit, originalLabel);
            originalLabel->show();
            edit->deleteLater();

            // 清理临时存储
            originalWidgets.remove(field);
            originalSizePolicies.remove(field);
            originalSizeHints.remove(field);
        }
    }

    // 如果是提交模式，发送更新请求
    if (!isEditable && !myusername.isEmpty()) {
        // 收集所有字段的值
        QString password = currentPassword;
        QString number = ui->numberLabel->text();
        QString school = ui->schoolLabel->text();
        QString phone = ui->phoneLabel->text();

        // // 获取密码（可能是新输入的或原有的）
        // QLineEdit* passwordEdit = findChild<QLineEdit*>("passwordLineEdit");
        // if (passwordEdit && !passwordEdit->text().isEmpty()) {
        //     password = passwordEdit->text();
        // } else {
        //     password = currentUserInfo["password"].toString(); // 这里应该从服务器获取真实密码或保持原密码
        //     // 实际应用中应该避免硬编码密码，这里需要改进
        // }

        // 构建用户信息JSON对象
        QJsonObject userInfo;
        userInfo["password"] = password;
        userInfo["number"] = number;
        userInfo["school"] = school;
        userInfo["phone"] = phone;

        // 发送更新请求
        client.updateUserInfo(myusername, userInfo);

        // 显示正在更新提示
        QMessageBox::information(this, "提示", "正在更新用户信息...");
    }
}

//弃置的函数
void MainWindow::on_checkloveButton_clicked(){}

//获取收藏的按钮
void MainWindow::on_pushButton_clicked()
{
    if (!isFavoritesView) {
        QMessageBox::information(this, "成功", "我的收藏已更新");
        client.requestFavorites(myusername);
        ui->pushButton->setText("刷新收藏");
        isFavoritesView = true;
    } else {
        QMessageBox::information(this, "成功", "我的收藏刷新成功");
        client.requestFavorites(myusername);
        ui->pushButton->setText("刷新收藏");
        isFavoritesView = true;
    }
}

//搜索按钮
void MainWindow::on_pushButton_2_clicked()
{
    QString keyword = ui->searchLineEdit->text();
    if (!client.isConnected()) {
        if (!client.connectToServer(qApp->property("IP").toString(), 1234)) {
            QMessageBox::critical(this, "网络错误", "无法连接到服务器，请检查网络设置");
            return;
        }
    }

    QJsonObject requestObj;
    requestObj["type"] = "SEARCH";
    requestObj["keyword"] = keyword;
    QJsonDocument doc(requestObj);
    QByteArray request = doc.toJson(QJsonDocument::Compact);
    client.sendRequest(request);
}

//主页刷新按钮
void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox::information(this, "提示", "刷新成功...");
    loadAllPosts();
}

